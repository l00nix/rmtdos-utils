#!/bin/sh
set -eu

if [ "$#" -lt 2 ]; then
  echo "usage: $0 VERSION ASSET_DIR [USER@HOST] [DEST_DIR]" >&2
  exit 2
fi

version="$1"
asset_dir="$2"
target="${3:-arau@shop}"
dest="${4:-rmtdos-utils}"

if [ ! -d "$asset_dir" ]; then
  echo "asset directory not found: $asset_dir" >&2
  exit 1
fi

ssh_cmd="ssh"
scp_cmd="scp"
if [ "${SSHPASS_FILE:-}" ]; then
  ssh_cmd="sshpass -f $SSHPASS_FILE ssh"
  scp_cmd="sshpass -f $SSHPASS_FILE scp"
fi

$ssh_cmd "$target" "mkdir -p '$dest'"
$scp_cmd "$asset_dir"/* "$target:$dest/"

$ssh_cmd "$target" "
  set -eu
  cd '$dest'
  cp -f 'rmtdos-utils-$version-linux-x86_64' rmtdos-utils
  cp -f 'cgaweb-$version.com' cgaweb.com
  cp -f 'cga_demo-$version.com' cga_demo.com
  cp -f 'vga_demo-$version.com' vga_demo.com
  rm -f rmtdos-cga-web-client rmtdos-file-commander
  chmod +x rmtdos-utils rmtdos-utils-$version-linux-x86_64
  chmod +x *.com
  sha256sum -c SHA256SUMS
"
