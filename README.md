# rmtdos-utils

`rmtdos-utils` is the umbrella home for the modern rmtdos utility family:

- `tools/cga-web`: DOS TSR, Linux remote-control client, CGA web viewer, and
  demo programs from `rmtdos-cga-web`.
- `tools/file-commander`: dual-pane ncurses file manager for a Linux machine
  talking to a DOS host running the `cgaweb.com` TSR.

The repository is based on the committed history of
[`l00nix/rmtdos-cga-web`](https://github.com/l00nix/rmtdos-cga-web), which is a
fork of Dennis Jenkins' original
[`dennisjenkins75/rmtdos`](https://github.com/dennisjenkins75/rmtdos).

## Layout

```text
tools/
  cga-web/          DOS TSR, Linux client, CGA web viewer, demos
  file-commander/   ncurses dual-pane DOS/Linux file manager
```

Each tool keeps its own README and build instructions. The root Makefile offers
convenience targets for building, cleaning, and formatting the whole collection.

## Building

Build everything:

```sh
make
```

Build a single tool:

```sh
make cga-web
make file-commander
```

The individual projects still own their detailed dependency notes:

- [tools/cga-web/README.md](tools/cga-web/README.md)
- [tools/file-commander/README.md](tools/file-commander/README.md)

## Project Lineage

`rmtdos-utils` keeps the `rmtdos-cga-web` git history as its base so the DOS TSR,
protocol, and CGA web work remain traceable. `rmtdos-file-commander` is imported
under `tools/file-commander` so its standalone development can live in the same
source tree as the protocol and TSR it depends on.

See [NOTICE.md](NOTICE.md) for attribution notes.
