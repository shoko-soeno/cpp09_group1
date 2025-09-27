## Package のインストール方法

1. submodule をアップデートする

```bash
git submodule update --init
```

2. パッケージを **build** してインストールする

```bash
# インストールする先のディレクトリパス
INSTALL_DIR="$(pwd)/packages"

PACKAGE_DIR_LIST=(
  "googletest"
)
for PACKAGE_DIR in $PACKAGE_DIR_LIST; do
  pushd . > /dev/null
  cd "$PACKAGE_DIR"
  mkdir build && cd $_
  cmake .. -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR"
  make -j$(nproc)
  make install
  popd > /dev/null
done
```
