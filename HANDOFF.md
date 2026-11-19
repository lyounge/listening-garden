# Listening Garden handoff
Updated 2026-11-13.
- Only game source: share/ListeningGarden.c. Keep beginner-friendly C11 and UTF-8.
- Playable Windows build: share/GardenClass.exe. Rebuild when changing game behavior.
- Removed obsolete root multi-file game and its EXE. Old versions remain in Git history and backup ZIPs.
- tests/test_garden.c includes the single-file game with main renamed for rule tests. It is development tooling, not needed to distribute the game.
- Build: gcc -std=c11 -Wall -Wextra -Werror -pedantic share/ListeningGarden.c -o GardenClass.exe
- Test: gcc -std=c11 -Wall -Wextra -Werror -pedantic tests/test_garden.c -o test_garden.exe
- Verified locally: both strict builds, 10000 randomized days including recipes/scoring/undo, full console round to score.
- GitHub workflow checks the same source and tests. Keep main protected, submit PRs.
- Run scripts/backup.ps1 before edits. ZIPs live outside repo under LocalAppData/ListeningGarden/Backups.
- This cleanup does not change game source or behavior. Pending: GitHub CI and merge of codex/single-game.

## 2026-11-19 Dialogue revision
Rewrote all 12 customer records, including both question paths, help requests/actions, and morning callbacks. Voice: concrete everyday speech, distinct reactions, no instant resolution required. Used kien-thai; native-model route unavailable, drafted and audited directly.
Only story data changed; game logic is byte-for-byte unchanged after normalizing the story block. Strict C11 game/test builds and 10000-day tests passed. Three complete console rounds covering dialogue options 1/2/3 passed with eight crystal gifts each. Rebuilt share/GardenClass.exe.
Removed user-authorized untracked exercises test1-4 (.c/.exe), tesst5.c, EmployeeData.txt and LibraryBooks.txt from share. Backup before removal includes these files in LocalAppData/ListeningGarden/Backups/garden-20261119-164751-7434524-97f5edc.zip.
