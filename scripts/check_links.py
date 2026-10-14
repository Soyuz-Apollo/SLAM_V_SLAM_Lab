"""Check internal Obsidian links and Markdown file/image links in lab notes."""

from collections import defaultdict
from pathlib import Path
import re
import sys
from urllib.parse import unquote, urlsplit


ROOT = Path(__file__).resolve().parents[1]
IGNORED = {".git", ".obsidian", "templates", "code", "data", "build", "__pycache__"}


def main():
    files = [p for p in ROOT.rglob("*") if p.is_file() and ".git" not in p.relative_to(ROOT).parts]
    by_name = defaultdict(list)
    for path in files:
        by_name[path.name].append(path)
        if path.suffix == ".md":
            by_name[path.stem].append(path)
    errors = []
    checked = 0
    notes = [p for p in files if p.suffix == ".md" and not (set(p.relative_to(ROOT).parts) & IGNORED)]
    for note in notes:
        content = re.sub(r"(?ms)^\s*(`{3,}|~{3,}).*?^\s*\1\s*$", "", note.read_text(encoding="utf-8"))
        content = re.sub(r"`+[^`\n]+`+", "", content)
        for match in re.finditer(r"!?\[\[([^\]\n]+)\]\]", content):
            target = match.group(1).split("|", 1)[0].split("#", 1)[0].strip()
            if not target:
                continue
            checked += 1
            resolved = note.parent / target
            absolute = ROOT / target
            if any(p.is_file() for p in (resolved, absolute, Path(str(resolved) + ".md"), Path(str(absolute) + ".md"))):
                continue
            matches = by_name.get(target, []) if "/" not in target else []
            if len(matches) != 1:
                errors.append(f"{note.relative_to(ROOT)}: unresolved or ambiguous wiki link: {target}")
        for match in re.finditer(r"!?\[[^\]\n]*\]\((<[^>]+>|[^\s)]+)(?:\s+\"[^\"]*\")?\)", content):
            target = unquote(match.group(1).strip("<>"))
            parsed = urlsplit(target)
            if parsed.scheme or parsed.netloc or not parsed.path:
                continue
            checked += 1
            destination = (ROOT / parsed.path.lstrip("/")) if parsed.path.startswith("/") else note.parent / parsed.path
            if not destination.exists():
                errors.append(f"{note.relative_to(ROOT)}: missing Markdown target: {target}")
    for error in errors:
        print(error, file=sys.stderr)
    print(f"Checked {checked} internal links in {len(notes)} notes; {len(errors)} errors.")
    print("External URLs, headings and historical command paths are not validated.")
    return bool(errors)


if __name__ == "__main__":
    sys.exit(main())
