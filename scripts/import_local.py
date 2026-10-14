"""Copy the existing research vault without modifying its files."""

import argparse
import json
from pathlib import Path
import shutil
import subprocess


MONO = "Davison_A_J_et_al_2007_MonoSLAM_Real_Time_Single_Camera_SLAM"
COMPARISON = "Sharafutdinov_D_et_al_2023_Comparison_of_modern_open_source_Visual_SLAM_approaches"
SHARED = (
    "slam_metrics_explained_ru.md",
    "slam_probability_error_derivation_ru.md",
    "slam_basic_terms_ground_truth_se3_rms.md",
)
SKIP_DIRS = {".git", ".idea", ".obsidian", "build", "CMakeFiles", "__pycache__", "data", "reference"}
SKIP_SUFFIXES = {".o", ".a", ".so", ".dylib", ".exe", ".bag", ".mcap", ".mp4", ".avi", ".mov", ".zip", ".gz"}


def ignored(directory, names):
    result = []
    for name in names:
        path = Path(directory) / name
        if (name in SKIP_DIRS or name.startswith(("cmake-build-", "build-"))
                or name in {".DS_Store", "CMakeCache.txt", "compile_commands.json"}
                or path.is_symlink() or path.suffix.lower() in SKIP_SUFFIXES):
            result.append(name)
        elif path.is_file():
            with path.open("rb") as stream:
                magic = stream.read(4)
            if magic in {b"\x7fELF", b"\xcf\xfa\xed\xfe", b"\xfe\xed\xfa\xcf", b"\xca\xfe\xba\xbe"}:
                result.append(name)
    return result


def git_info(path):
    def run(*args):
        process = subprocess.run(["git", "-C", str(path), *args], capture_output=True, text=True)
        if process.returncode:
            raise RuntimeError(f"Cannot read Git provenance for {path}: {process.stderr.strip()}")
        return process.stdout.strip()
    return {"base_commit": run("rev-parse", "HEAD"), "local_changes": run("status", "--short").splitlines()}


def write_json(path, value):
    path.write_text(json.dumps(value, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, required=True)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    source = args.source.resolve()
    articles = source / "research paper"
    code_source = source / "monoslam_code"
    if (root / "migration_report.json").exists():
        parser.error("Import already completed; refusing to overwrite existing materials.")
    for paper in (MONO, COMPARISON):
        if not (articles / paper).is_dir() or (root / "papers" / paper).exists():
            parser.error(f"Missing source or existing destination: {paper}")
    if not (articles / "datasets").is_dir():
        parser.error("Dataset notes directory is missing.")

    for paper in (MONO, COMPARISON):
        shutil.copytree(articles / paper, root / "papers" / paper, ignore=ignored)
    for name in SHARED:
        shutil.move(str(root / "papers" / COMPARISON / name), root / "notes" / name)
    for path in sorted((articles / "datasets").glob("*.md")):
        shutil.copy2(path, root / "datasets" / path.name)

    code_dest = root / "papers" / MONO / "code"
    legacy = code_dest / "legacy"
    legacy.mkdir(parents=True)
    for name in ("SceneLib", "MonoSLAMGlow", "VW34", "glow_104"):
        shutil.copytree(code_source / name, legacy / name, ignore=ignored)
    for name in ("CMakeLists.txt", "mac_camera_33_5mm.xml"):
        shutil.copy2(code_source / name, legacy / name)
    shutil.copy2(code_source / "MonoSLAM_run_commands_ru.md", root / "papers" / MONO / "MonoSLAM_run_commands_ru.md")

    implementations = []
    for folder, url in (
        ("github_marcromani_MonoSLAM", "https://github.com/marcromani/MonoSLAM"),
        ("github_rrg-polito_mono-slam", "https://github.com/rrg-polito/mono-slam"),
    ):
        info = git_info(code_source / folder)
        shutil.copytree(code_source / folder, code_dest / folder, ignore=ignored)
        implementations.append({"path": folder, "upstream_url": url, "kind": "third_party", **info})
    write_json(code_dest / "import_report.json", {
        "source_directory": str(code_source),
        "snapshot_type": "working_tree_without_git_history",
        "legacy": {"path": "legacy", "kind": "original_legacy_code_with_local_build_changes", "upstream_url": "https://www.doc.ic.ac.uk/~ajd/Scene/", "base_commit": None, "license_status": "review_required"},
        "implementations": implementations,
        "build_verified_after_import": False,
        "excluded": ["nested .git", "build outputs", "raw datasets", "reference PDFs", "symlinks"],
    })

    # Replace only the known old code-root prefixes, preserving filenames.
    for old_root in ("/Users/m/Desktop/obs_avoidong/monoslam_code", str(code_source)):
        replacements = {
            old_root + "/github_marcromani_MonoSLAM": f"papers/{MONO}/code/github_marcromani_MonoSLAM",
            old_root + "/github_rrg-polito_mono-slam": f"papers/{MONO}/code/github_rrg-polito_mono-slam",
            old_root + "/TestSeqMonoSLAM": "data/monoslam/TestSeqMonoSLAM",
            old_root + "/tello_test": "data/monoslam/tello_test",
            old_root: f"papers/{MONO}/code/legacy",
        }
        for path in (root / "papers" / MONO).rglob("*.md"):
            content = path.read_text(encoding="utf-8")
            updated = content
            for old, new in replacements.items():
                updated = updated.replace(old, new)
            if updated != content:
                path.write_text(updated, encoding="utf-8")

    for paper, title in (
        (MONO, "MonoSLAM: Real-Time Single Camera SLAM"),
        (COMPARISON, "Comparison of modern open-source Visual SLAM approaches"),
    ):
        write_json(root / "papers" / paper / "metadata.json", {
            "paper_id": paper,
            "title": title,
            "source": {"kind": "local_import", "original_directory": str(articles / paper)},
            "collection_url": "https://disk.360.yandex.ru/d/IgLFLct2HZKq_Q",
            "collection_membership_verified": False,
            "original_pdf": next((root / "papers" / paper).glob("*.pdf")).name,
            "translation_status": "imported_not_audited",
            "reproduction_status": "not_verified_after_import",
        })
        (root / "papers" / paper / "experiments").mkdir()
    (code_dest / "containers").mkdir()
    write_json(root / "migration_report.json", {
        "source": str(source),
        "paper_count": 2,
        "dataset_note_count": len(list((articles / "datasets").glob("*.md"))) - 1,
        "shared_note_count": len(SHARED),
        "imported_file_count": sum(path.is_file() for path in (root / "papers").rglob("*")),
        "raw_datasets_imported": False,
        "source_modified": False,
    })
    print("Imported 2 papers, 13 dataset descriptions, shared notes and MonoSLAM code.")


if __name__ == "__main__":
    main()
