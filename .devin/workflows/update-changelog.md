---
description: Update the project changelog for a new release
---

# Changelog Update Workflow

Use this workflow when preparing a new release to update the CHANGELOG.md and VERSION files.

> **Format Reference**: Follow [Keep a Changelog](https://keepachangelog.com/en/1.0.0/) format — group changes into Added, Changed, Deprecated, Removed, Fixed, and Security sections.

## Steps

1. **Identify the version bump**
   - Read `VERSION` file to determine current version
   - Decide new version based on Semantic Versioning:
     - **MAJOR** (x.0.0): Breaking API changes
     - **MINOR** (0.x.0): New features, backwards compatible
     - **PATCH** (0.0.x): Bug fixes only

2. **Review git changes since last release**
   ```bash
   git log --oneline <last-tag>..HEAD
   git diff --name-status <last-tag>..HEAD
   ```

3. **Categorize changes into relevant sections**
   Only create sections that have actual changes. Common categories:
   - **Added**: New features, capabilities, files
   - **Changed**: Modifications to existing behavior/API
   - **Deprecated**: Soon-to-be removed features (omit if none)
   - **Removed**: Deleted files/features (omit if none)
   - **Fixed**: Bug fixes (omit if none)
   - **Security**: Security-related fixes (omit if none)

4. **Update VERSION file**
   ```bash
   echo "X.Y.Z" > VERSION
   ```

5. **Update CHANGELOG.md**
   - Insert new version section at top (after Unreleased if present)
   - Use format: `## [X.Y.Z] - YYYY-MM-DD`
   - Group entries by category (Added/Changed/Fixed/etc.)
   - Keep entries concise but descriptive
   - Link to issues/PRs when relevant: `(#123)`

6. **Verify the changes**
   - Check for complete sentences
   - Verify date is today's date
   - Ensure consistent formatting with previous entries

## Format Example

```markdown
## [0.4.0] - 2026-06-10

### Added
- Math expression typesetting on LCD canvas
- Unit tests for layout engine

### Changed
- Refactored input handling for text-first architecture

### Fixed
- Display rendering with proper font metrics
```

## Post-Update

1. Stage changes: `git add VERSION CHANGELOG.md`
2. Commit with message: `chore: bump version to X.Y.Z`
3. Create git tag: `git tag vX.Y.Z`
4. Push: `git push && git push --tags`

## Quick Reference

| Section | Use When | Omit When |
|---------|----------|-------------|
| `### Added` | New features/files | No new features |
| `### Changed` | API/behavior changes | No breaking changes |
| `### Fixed` | Bugs fixed | No bug fixes |
| `### Deprecated` | Marking for removal | No deprecations |
| `### Removed` | Deleted features | Nothing removed |
| `### Security` | CVE/patches | No security fixes |

## Notes

- Keep the `[Unreleased]` section at the top for ongoing work
- Remove empty sections (e.g., no `### Deprecated` if nothing deprecated)
- Be consistent with verb tense (imperative: "Add", "Fix", "Change")
- Don't include commit hashes in changelog entries
- Don't include trivial changes (whitespace, formatting, comments)
- Security fixes should always be explicitly noted

## Reference

- Follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/)
- Uses [Semantic Versioning](https://semver.org/spec/v2.0.0.html)
- VERSION file format: `MAJOR.MINOR.PATCH` (no leading 'v')
- Date format: ISO 8601 (`YYYY-MM-DD`)
