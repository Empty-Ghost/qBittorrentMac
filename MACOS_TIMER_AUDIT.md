# macOS timer and periodic-work audit

Audited: 2026-07-18

This inventory classifies application timers by whether macOS may coalesce their wakeups. It records the first energy-efficiency pass; Instruments measurement is still required before claiming a quantified improvement.

| Area | Timer purpose | Selected behavior | Reason |
| --- | --- | --- | --- |
| Settings storage | Batch settings writes for 5 seconds | `Qt::CoarseTimer` | A small delay is acceptable and destruction still flushes dirty state. |
| File logger | Batch log writes for 2 seconds | `Qt::CoarseTimer` | A small delay is acceptable and close still flushes the file. |
| Session | Enforce seeding limits every 10 seconds | `Qt::CoarseTimer` | Sub-second precision is not required for minute-based limits. |
| Session | Check free disk space every 30 seconds | `Qt::CoarseTimer` | Small scheduling variance does not change the safety threshold. |
| Session | Save resume data at a user-selected minute interval | `Qt::VeryCoarseTimer` | Persistence is periodic and shutdown performs its own save path. |
| Session | Refresh additional tracker URL every 24 hours | `Qt::VeryCoarseTimer` | This is background maintenance with no exact deadline. |
| Session | Detect wake from sleep every 30 seconds | `Qt::CoarseTimer` | Detection already uses a 100-second threshold. |
| Dynamic DNS | Check the public IP every 30 minutes | `Qt::VeryCoarseTimer` | The service already prevents rapid updates. |
| RSS session | Refresh feeds at their configured timepoints | `Qt::CoarseTimer` | Feed updates tolerate small variance while preserving per-feed schedules. |
| RSS storage | Batch feed and auto-downloader writes for 5 seconds | `Qt::CoarseTimer` | These are deferred persistence operations with explicit destruction-time stores. |
| Watched folders | Poll and retry failed torrent files every 10 seconds | `Qt::CoarseTimer` | Filesystem events remain immediate; only fallback polling is coalesced. |
| Program updates | Initial and daily automatic checks | Five-minute startup delay, then 24-hour `Qt::VeryCoarseTimer` | Avoids launch contention while retaining automatic checks. |
| Power management | Reconcile assertion state every 10 minutes | `Qt::VeryCoarseTimer` | Torrent lifecycle signals provide immediate updates; the timer is only a safety net. |
| Torrent/session stats | Refresh libtorrent state | Existing `Qt::CoarseTimer` | This is user-configurable interactive UI data. |
| Torrent watcher | Debounce filesystem notifications for 2 seconds | Existing `Qt::CoarseTimer` | This is already coalescible. |
| Web UI shutdown | Delay application shutdown for 100 milliseconds | Existing `Qt::CoarseTimer` | This is already coalescible. |
| Splash screen | Remove splash after 1.5 seconds | Existing `Qt::CoarseTimer` | This is already coalescible. |
| Search | Enforce search timeout | Precise default retained | This is a user-visible deadline. |
| Web server | Drop timed-out connections | Precise default retained | Changing network timeout behavior needs separate load tests. |
| Bandwidth scheduler | Detect configured schedule boundaries | Precise default retained | Late switching changes user-requested bandwidth behavior. |
| Tracker details | Refresh visible announce countdown | Precise default retained | This is visible UI state and should stop with the model. |
| Filter input | Debounce typing for 400 milliseconds | Precise default retained | This is directly interactive. |
| Shutdown dialog | Update countdown each second | Precise default retained | This is visible and controls shutdown timing. |
| RSS auto-downloader | Advance queued jobs on the next event turn | Precise zero-delay retained | It sequences work rather than polling. |
| Recent torrent errors | Clear the short-lived deduplication set after 1 second | Precise default retained | This bounds duplicate notifications and is not a continuously active timer. |
| Download manager | Pace sequential service jobs | Precise default retained | Per-service delays may be protocol or rate-limit requirements. |

## Follow-up validation

- Compare idle wakeups and timer fires with RSS, Web UI, search, and watched folders enabled and disabled.
- Confirm settings, logs, and resume data are flushed on normal quit and forced-termination recovery remains unchanged.
- Verify update checks do not run during session restoration and recur after a completed manual or automatic check.
- Revisit precise timers only with component-specific correctness and latency tests.
