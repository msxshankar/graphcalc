## 2026-06-06T22:51:09Z

<user_information>
The USER's OS version is linux.
The user has 1 active workspaces, each defined by a URI and a CorpusName. Multiple URIs potentially map to the same CorpusName. The mapping is shown as follows in the format [URI] -> [CorpusName]:
/home/mayur/Documents/repos/graphcalc -> msxshankar/graphcalc
Code relating to the user's requests should be written in the locations listed above. Avoid writing project code files to tmp, in the .gemini dir, or directly to the Desktop and similar folders unless explicitly asked.
App Data Directory: /home/mayur/.gemini/antigravity
Conversation ID: f700e2cf-448b-4774-b068-2d0fbaced511
</user_information><subagent_reminder>
You are running as a subagent, invoked by a caller agent (name: "main agent", id: "1c9f1421-6fc0-488a-b54b-e1a602d5cc4e"). You MUST use send_message to communicate all results, reports, and updates back to the caller. Your response is NOT automatically relayed — if you do not call send_message, the caller will only know that you have gone idle. Always use the caller's id as the Recipient and "main agent" as the RecipientName.

Text you generate outside of send_message will NOT be seen by the caller, so keep them brief. Put all important information — findings, summaries, conclusions — into your send_message calls instead. You can also share files by including their absolute paths in your message; the caller can then read them directly.
</subagent_reminder><subagents>
Available subagents:
- self: Subagent that inherits the parent agent's full configuration including tools, system prompt, and model. Use this when you need to run a task in a separate conversation context but with the same capabilities as the current agent.
- teamwork_preview_orchestrator: Pure orchestrator. Dispatches tasks to specialists, monitors progress, and synthesizes results. Does not write code directly.
- teamwork_preview_victory_auditor: Independent post-victory auditor. Spawned by the Sentinel when the team claims project completion. Conducts a 3-phase audit (timeline, cheating detection, independent test execution) with zero shared context from the implementation swarm. Reports a structured verdict.

After launching a subagent, you do NOT need to poll or check your inbox in a loop. The system will automatically notify you when the subagent sends a message. Simply proceed with other work or stop calling tools, and you will be notified when there is a message to process.

</subagents><USER_REQUEST>
The project involves expanding the description of every 2D, 3D, and visualization template in the `graphcalc` codebase to include the relevant mathematics and history behind it, and adding scrolling capabilities for these descriptions in the TUI menu.

Working directory: /home/mayur/Documents/repos/graphcalc
Integrity mode: development

## Requirements

### R1. Mathematical & Historical Descriptions
- Expand all descriptions in `templates2D`, `templates3D`, and `templatesVis` in `src/tui.cpp` (and any other menu files) into natural, flowing paragraphs.
- Descriptions should explain the mathematics and history of the curve, surface, or visualization, written for a general audience that has some math knowledge.

### R2. Description Scrolling in TUI
- Implement vertical scrolling for the description box in the TUI menu.
- If a description is too long to fit in the available vertical space below the preview box, the TUI should display a scrollbar or indicator and allow the user to scroll up and down using the `[` and `]` keys.
- Changing the selected template or switching tabs should reset the scroll offset back to 0.

## Acceptance Criteria

### Content Quality
- [ ] Every template has a description containing at least 80 characters of detailed math and history explanation.
- [ ] Explanations are written as natural-flowing prose, not bullet points or simple definitions.

### Scrolling & Layout
- [ ] Pressing `[` scrolls the current description up, and `]` scrolls it down.
- [ ] Description text wraps correctly inside the right-column boundary without overflowing.
- [ ] Switching between tabs or templates resets the description scroll position to the top.
- [ ] The codebase compiles cleanly with `make clean && make`.
</USER_REQUEST>
