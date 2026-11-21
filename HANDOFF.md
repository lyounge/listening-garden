# Listening Garden handoff
Updated 2026-11-13.
- Main game source: GrowGarden/Main/share/ListeningGarden.c. Keep beginner-friendly C11 and UTF-8.
- Playable Windows build: GrowGarden/Main/share/GardenClass.exe. Rebuild when changing game behavior.
- Removed obsolete root multi-file game and its EXE. Old versions remain in Git history and backup ZIPs.
- tests/test_garden.c includes the single-file game with main renamed for rule tests. It is development tooling, not needed to distribute the game.
- Build: gcc -std=c11 -Wall -Wextra -Werror -pedantic GrowGarden/Main/share/ListeningGarden.c -o GardenClass.exe
- Test: gcc -std=c11 -Wall -Wextra -Werror -pedantic tests/test_garden.c -o test_garden.exe
- Verified locally: both strict builds, 10000 randomized days including recipes/scoring/undo, full console round to score.
- GitHub workflow checks the same source and tests. Keep main protected, submit PRs.
- Run scripts/backup.ps1 before edits. ZIPs live outside repo under LocalAppData/ListeningGarden/Backups.
- This cleanup does not change game source or behavior. Pending: GitHub CI and merge of codex/single-game.

## 2026-11-19 Dialogue revision
Rewrote all 12 customer records, including both question paths, help requests/actions, and morning callbacks. Voice: concrete everyday speech, distinct reactions, no instant resolution required. Used kien-thai; native-model route unavailable, drafted and audited directly.
Only story data changed; game logic is byte-for-byte unchanged after normalizing the story block. Strict C11 game/test builds and 10000-day tests passed. Three complete console rounds covering dialogue options 1/2/3 passed with eight crystal gifts each. Rebuilt GrowGarden/Main/share/GardenClass.exe.
Removed user-authorized untracked exercises test1-4 (.c/.exe), tesst5.c, EmployeeData.txt and LibraryBooks.txt from share. Backup before removal includes these files in LocalAppData/ListeningGarden/Backups/garden-20261119-164751-7434524-97f5edc.zip.

## 2026-11-19 Console presentation
Title is Grow a (Feeling) Garden, with สวนรับฟัง as Thai subtitle. Compact decorative text tree on welcome screen; actual growth trees unchanged. Added heading() using printf, separated numbered menu rows, customer progress and clearer input prompt. No new headers or libraries.
Validation: strict C11 builds, 10000-day rule tests, direct-play and help paths through complete rounds passed. Windows GUI preview launch did not expose a window; console output verified programmatically. Rebuild the shipped EXE with each UI change.
User correction: restored original title tree, split help into morning/evening and single-column care notes. Updated shipped EXE using a recoverable rename of the running old executable. Final shipped EXE direct/help rounds and 10000-day tests passed. Thai glyph spacing remains terminal/font-dependent; no terminal settings changed.
2026-11-19: Welcome choices renamed to เริ่มเล่น (เปิดร้าน) and คู่มือเกม. Strict C11 build passed; rebuilt shipped EXE. Menu behavior unchanged.

## 2026-11-20 Clear instructions and results
Removed redundant Aunt Ing subtitle under start. Help explains mixing shortages/cost/example, survival versus optional Perfect flowers, all weather requirements and health damage. Forecasts now describe threats and requirements on separate lines. Dialogue continuation and crystal rewards separated clearly. Ending now shows result, actual values and deficits before a shorter Aunt reflection; returning customer has their own heading.
Gameplay and choice sequence unchanged. Strict C11 builds, 10000-day tests, all three conversation paths through full rounds (including help and result labels) passed. Rebuilt shipped EXE.

## 2026-11-20 Playtest improvements
Completed six requested changes: paged skippable help, weather-specific safe targets plus inventory on planting table, individual third dialogue choices, skip identical growth frames, minimum 10 in all three parts to survive, and deficit-specific ending reflections. Mixing stays in its separate menu; title tree unchanged.
Validation: strict C11 builds; 10000-day rules and new missing-part survival regressions across all weather; direct/all four help skips/full guide complete rounds; four representative growth plans had no repeated adjacent frames. Rebuilt EXE. README describes survival versus safe targets and Perfect.

## 2026-11-20 Presentation readiness pass
Two iteration review/playtest loop with independent reviewer. Added full/brief dialogue selection at shop opening, same gifts; continuous feeding with per-selection undo; score and result before returning visitor, Aunt closes story. Dead trees use existing bare-branch image; title tree unchanged. Removed unused proportions function.
Validation: strict C11 builds, 10000-day rules; EXE full and brief rounds to Perfect without in-game answer, consecutive feeds + single undo + reset, malformed input/EOF, deterministic full/brief gift equivalence. Independent final review found no blocking logic defect. Updated CI smoke sequence. Final binary rebuilt after removing unused function.
No graphics library or new headers. Known scope: terminal Thai font shaping remains device-dependent; 30/30/30 is intentionally universal Perfect target; no external human usability study performed.

## 2026-11-20 Final release hardening
Fixed reproduced long-line spill and integer overflow in choose (bounded two-digit scan, drain incomplete lines). Added wait_enter to consume complete pause lines. Full dialogue pauses after gifts; fixed line breaks in long story strings avoid runtime Unicode helpers. Added CODE-GUIDE.md for explaining every subsystem and presentation sequence.
tests/play_game.py permanently checks malformed/overflow/truncated input, full/brief rounds, every guide exit, no-fertilizer ending and inventory-planned Perfect. CI now runs these alongside 10000-day C rules. No new game headers.
2026-11-20: Role-based dialogue polish focused on question-answer continuity and natural speech. Updated all three C copies and rebuilt Windows EXE. Strict C11 builds and gameplay suite passed on each copy.
Dialogue continuity pass: edited all three copies, rebuilt EXE, checked all 36 character/choice combinations and six full EXE rounds. No rules changed.
Added optional weather challenges and live planting previews. Existing score and Perfect unchanged; the optional challenge may not be reachable with every inventory. Gameplay suite passed.
Weather-specific flowering targets and optional last-customer crystal exchange. Original inventory guaranteed Perfect; optional exchange can change feasibility. Strict builds, 10000-day tests, full gameplay and exchange smoke passed.
Added inventory-filtered recipe lookup and batch mixing with batch undo, streamlined planting targets, and restored gift attribution after trade. Collection deferred. Gameplay suite passed.
Trade offered only if revised Perfect solution works; mixing stays open; survival and health deficits shown. Build, menu tests and 10000 trade/normal inventories checked.
Combined duplicate planting hints per part, kept mixing open after quantity cancel (EOF still exits), and aligned trade callback text. Strict build and gameplay checks passed. Day counter deferred.
Clarified planting hints, flowering deficits and score, added Aunt Ing scene transitions and brief-mode label. Strict build and gameplay tests passed.
Removed brief mode. Shop opens with full dialogue only. All three strict C11 builds and menu tests passed.
Unified Perfect solver for day generation and trades; retained single-file game, art and dialogue. All three builds and gameplay suites plus 10000-day rules passed.
Removed trades, guest morning callbacks and per-batch undo; retained full refund and scores. Updated guides and gameplay tests.
Mini edition at GrowGarden/Mini/ListeningGarden-Mini.c is standalone (564 lines). Preserves story, ending and art data; simplified day generation and planting controls. Strict C11, complete empty round and 10000 solvable inventories verified.
Mini is the primary classroom project. Full edition retained for reference. Mini C source stays at 564 lines; added Mini explanation and CI smoke check.
Mini startup now offers a short optional guide. Strict build, guide return/exit and full round after reading passed.
Mini: 12 original console portraits, hidden running inventory in gift scene, and six direct fertilizer effect labels. Strict build and complete help-to-score run passed. Still under 600 lines.
Refined all 12 Mini portraits to nine rows of console art each. Strict build and complete round passed; 596 source lines.
Mini portraits now use distinct face shapes, eyes and accessories instead of a shared face. Strict build and full round passed; 596 lines.
Mini planting menu 5 lists all 12 recipes without consuming crystals. Strict build and recipe-to-ending smoke passed; 600 lines.
Added Mini team guide with gameplay, code flow, pointer explanation and the 100-point classroom rubric.
PR 36: fixed recipe switch indentation and trimmed blank lines and trailing whitespace. Strict compile and recipe-to-ending smoke passed.
Added comment-free Mini submission copy. Both strict builds passed; preprocessed output is identical. Edit the commented source first and refresh the submission copy.
