# Shop Deploy

Release assets are also copied to the shop Linux machine:

```text
arau@shop:~/rmtdos-utils
```

The folder keeps versioned release assets and convenient current names:

- `rmtdos-utils-vX.Y.Z-linux-x86_64`
- `cgaweb-vX.Y.Z.com`
- `cga_demo-vX.Y.Z.com`
- `vga_demo-vX.Y.Z.com`
- `SHA256SUMS`

Current convenience copies are updated during deploy:

- `rmtdos-utils`
- `cgaweb.com`
- `cga_demo.com`
- `vga_demo.com`

From WSL, with a password file supplied outside the repo:

```sh
SSHPASS_FILE=/path/to/password \
  scripts/deploy-shop-release.sh vX.Y.Z ../release-assets/rmtdos-utils-vX.Y.Z
```
