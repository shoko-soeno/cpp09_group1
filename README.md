## Package のインストール方法

1. submodule をアップデートする

```bash
git submodule update --init
```

2. パッケージを **build** してインストールする

```bash
# インストールする先のディレクトリパス
INSTALL_DIR="$(pwd)/packages"

pushd . > /dev/null
cd packages_github
for PACKAGE_DIR in $(ls -d */); do
  pushd . > /dev/null
  cd "$PACKAGE_DIR"
  mkdir build && cd $_
  cmake .. -DCMAKE_INSTALL_PREFIX="$INSTALL_DIR"
  make -j$(nproc)
  make install
  popd > /dev/null
done
popd > /dev/null
```
