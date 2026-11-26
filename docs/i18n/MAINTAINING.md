# Maintaining README translations

The [English README](../../README.md) is the source for every translation. Each translated README records the Git blob ID of the English file it was checked against in a `source-readme` HTML comment. The comment is visible in the Markdown source.

From the repository root, run this in PowerShell to list translations that need review:

```powershell
$englishReadme = git hash-object --path=README.md README.md
if ($LASTEXITCODE -ne 0) { throw 'Could not calculate the English README revision.' }

Get-ChildItem -LiteralPath 'docs/i18n' -Filter 'README.*.md' -File |
    Where-Object {
        -not (Select-String -LiteralPath $_.FullName -SimpleMatch "<!-- source-readme: $englishReadme -->" -Quiet)
    } |
    Select-Object -ExpandProperty Name
```

The command prints nothing when all revision markers match. After an English edit, review each listed translation against the new source. If you need the old English text, run `git show <old-blob-id>` in a clone that contains the old revision. Translate the changed sections, check links and headings, and replace the comment's ID with the output of `git hash-object --path=README.md README.md` once the translation is current.

Use the menu's existing localized labels where known. Preserve technology names, commands, paths, release details, and the distinction between earlier in-game testing and the current release's verification. Keep translated READMEs and their links in UTF-8.

[Installation](../INSTALLATION.md) is the source for the localized `INSTALLATION.<locale>.md` guides. They cover the release changes, setup, removal and a short verification procedure. The release ZIP includes these guides, and [the GitHub release text](../GITHUB_RELEASE.md) links to them. Ukrainian is available for documentation. In-game language selection follows Persona 3 Reload's supported locales.
