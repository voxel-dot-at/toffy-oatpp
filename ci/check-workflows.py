#!/usr/bin/env python3
"""Check GitHub workflow files for contexts used where they are not allowed.

Why this exists: `${{ env.TOFFY_REF }}` in a job's `name:` is not a step failure.
The `env` context is not available there (a job name sees github, needs, strategy,
matrix, vars, secrets), and GitHub does not report it as a failing job — it fails
the *workflow file*. The run is created, executes nothing, concludes `failure`, has
zero jobs, and is titled with the file path instead of the workflow's `name:`. This
repository's first CI run, 41358317729, looked exactly like that: no log to read,
no step to point at, and nothing in the Actions UI saying "the file did not parse".

A workflow that never ran and a workflow that failed are the same colour, and only
one of them has a diagnosis. So: check the file before pushing it.

Usage:  ci/check-workflows.py .github/workflows/*.yml
Exit:   0 clean, 1 at least one problem (each printed with its path)

Scope, stated so it is not mistaken for more: this checks *context position* (the
class that fails the whole file silently) and one step-shape error (`uses:` and
`run:` together). It does not validate action inputs, expression grammar, shell, or
anything about what the build does. `actionlint` covers much more and should replace
this the day it is available in the image — this exists because it was not, and
because the defect it catches is one sentence long and cost a red run nobody could
read.

Proven against the real thing: re-inserting the `${{ env.TOFFY_REF }}` job name it
was written for makes it exit 1 naming `env`; both this repository's and toffy's
workflows pass it.
"""
import re
import sys

import yaml

# Contexts allowed per key, from GitHub's "Contexts availability" table. Keys not
# listed here are unchecked (steps, where almost everything is available).
JOB_ALLOWED = {
    "name": {"github", "needs", "strategy", "matrix", "vars", "secrets"},
    "if": {"github", "needs", "strategy", "matrix", "vars", "secrets"},
    "runs-on": {"github", "needs", "strategy", "matrix", "vars", "secrets"},
    "needs": {"github", "needs", "strategy", "vars"},
    "env": {"github", "needs", "strategy", "matrix", "vars", "secrets"},
    "outputs": {"github", "needs", "strategy", "matrix", "vars", "secrets", "steps"},
    "permissions": {"github", "needs", "strategy", "matrix", "vars"},
    "container": {"github", "needs", "strategy", "matrix", "vars", "secrets"},
}
TOP_ALLOWED = {
    "env": {"github", "vars", "secrets"},
    "concurrency": {"github", "vars"},
    "permissions": {"github", "vars"},
}

EXPR = re.compile(r"\$\{\{(.*?)\}\}", re.S)
IDENT = re.compile(r"[A-Za-z_][A-Za-z0-9_-]*(?:\.[A-Za-z_0-9'()\[\]-]+)*")


def contexts(text):
    """Leading context name of every ${{ … }} reference in a scalar."""
    found = []
    for expr in EXPR.findall(str(text)):
        expr = re.sub(r"'[^']*'", " ", expr)  # string literals are not contexts
        for match in IDENT.findall(expr):
            head = match.split(".")[0]
            if head:
                found.append(head)
    return found


def walk(path, node, allowed, problems):
    if isinstance(node, dict):
        for key, value in node.items():
            walk(path + [str(key)], value, allowed, problems)
    elif isinstance(node, list):
        for index, value in enumerate(node):
            walk(path + [str(index)], value, allowed, problems)
    elif allowed is not None:
        for context in contexts(node):
            if context not in allowed:
                problems.append((".".join(path), context, sorted(allowed)))


def check_file(name, problems):
    with open(name) as handle:
        doc = yaml.safe_load(handle)
    if not isinstance(doc, dict):
        problems.append((name, "file is not a mapping", []))
        return

    for key, allowed in TOP_ALLOWED.items():
        if key in doc:
            walk([name, key], doc[key], allowed, problems)

    for job_id, job in (doc.get("jobs") or {}).items():
        if not isinstance(job, dict):
            continue
        for key, allowed in JOB_ALLOWED.items():
            if key in job:
                walk([name, "jobs", job_id, key], job[key], allowed, problems)
        for index, step in enumerate(job.get("steps") or []):
            if isinstance(step, dict) and "uses" in step and "run" in step:
                problems.append(
                    (f"{name}:jobs.{job_id}.steps[{index}]", "uses: and run: together", [])
                )


def main(argv):
    if len(argv) < 2:
        print(__doc__)
        return 2
    problems = []
    for path in argv[1:]:
        check_file(path, problems)
    if not problems:
        print(f"{len(argv) - 1} workflow file(s): no context-position problems")
        return 0
    for where, context, allowed in problems:
        print(f"PROBLEM {where}\n    context '{context}' is not available here"
              f"\n    allowed: {', '.join(allowed)}")
    return 1


if __name__ == "__main__":
    sys.exit(main(sys.argv))
