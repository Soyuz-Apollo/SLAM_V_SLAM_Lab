# GitHub

## Проверенное состояние на 2026-10-14

- `gh api user --jq .login` подтверждает авторизацию как `Soyuz-Apollo`.
- Репозиторий: [Soyuz-Apollo/SLAM_V_SLAM_Lab](https://github.com/Soyuz-Apollo/SLAM_V_SLAM_Lab).
- Репозиторий приватный, основная ветка `main`.
- `origin` использует HTTPS: `https://github.com/Soyuz-Apollo/SLAM_V_SLAM_Lab.git`.
- Начальная история GitHub и локальная история лаборатории объединены обычным merge с сохранением обеих историй.

Авторизация подключённого приложения и авторизация `git`/`gh` на компьютере независимы. Git сам по себе ключ не требует; он нужен для доступа к удалённому серверу.

## Работа с существующим репозиторием

Репозиторий уже существует; повторять `gh repo create` не нужно. Для нового компьютера:

```bash
gh auth login --hostname github.com --web --git-protocol https
gh auth setup-git
gh repo clone Soyuz-Apollo/SLAM_V_SLAM_Lab
```

Перед изменениями в существующей копии выполни `git pull --ff-only`, затем проверь `git status --short`. После проверки изменений добавь конкретные файлы, создай commit и отправь его:

```bash
git add <file_paths>
git commit -m "Describe the change"
git push origin main
```

`<file_paths>` обозначает пути изменённых файлов. При расхождении историй сначала проверь коммиты обеих сторон и объедини их; принудительная отправка для обычной работы не нужна.

Не сохраняй токены или приватные ключи в репозитории, заметках и сообщениях чата.
