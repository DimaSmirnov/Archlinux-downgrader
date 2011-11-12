# Author: Dmitriy Smirnov <other@igus.lv>
pkgname=downgrader
pkgver=1.1.2
pkgrel=1
pkgdesc="Powerful packages downgrader for Archlinux, powered by libalpm. Works with AUR, ARM and pacman logs"
arch=('any')
depends=('git' 'pacman' 'intltool')
license=("GPL")
url="https://github.com/DimaSmirnov/Archlinux-downgrader"
source=("http://www.igus.lv/downgrader/downgrader.tar.xz")
md5sums=('f58cc78711e262f0f610095912655ce6')

build() {
  cd "$srcdir"
 make
}
package() {
  cd "$srcdir"
  install -D downgrader "$pkgdir/usr/bin/downgrader"
}
