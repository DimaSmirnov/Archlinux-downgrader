# Author: Dmitriy Smirnov <other@igus.lv>
pkgname=downgrader
pkgver=1.2.0
pkgrel=1
pkgdesc="Powerful packages downgrader for Archlinux, powered by libalpm. Works with AUR, ARM and pacman logs"
arch=('any')
depends=('pacman' 'intltool')
license=("GPL")
url="https://github.com/DimaSmirnov/Archlinux-downgrader"
source=("http://www.igus.lv/downgrader/downgrader.tar.xz")

build() {
  cd "$srcdir"
 make
}
package() {
  cd "$srcdir"
  install -D downgrader "$pkgdir/usr/bin/downgrader"
}
md5sums=('e3eeed11df142369c1339bc5b796fd59')
