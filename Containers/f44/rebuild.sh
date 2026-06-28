#!/usr/bin/env bash
set -e

DIR_NAME=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
IMAGE_NAME="weng-dev:f44"
TLBX_NAME_NAME="weng-dev-f44"
EXTERNAL_REPOS_PATH="$DIR_NAME/../../External"

cd "$DIR_NAME"

echo "🧱 Building container image..."
podman build -t "$IMAGE_NAME" .

echo "🧹 Removing old toolbox if it exists..."
toolbox rm -f "$TLBX_NAME_NAME" 2>/dev/null || true

echo "📦 Creating toolbox from image..."
toolbox create --image "$IMAGE_NAME" "$TLBX_NAME_NAME"

echo "🧹 Prune old podman images..."
podman image prune -f

echo "✅ Done!"
echo "Enter with: toolbox enter $TLBX_NAME_NAME"
