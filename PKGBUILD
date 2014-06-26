# Author: Dmitriy Smirnov <other@igus.lv>
pkgname=downgrader
pkgver=1.7.0
pkgrel=5
pkgdesc="Powerful packages downgrader for Archlinux. Works with libalpm, ARM and pacman logs"
arch=('any')
depends=('pacman' 'intltool' 'sudo')
license=("GPL")
url="https://github.com/DimaSmirnov/Archlinux-downgrader"
source=("http://www.igus.lv/downgrader/downgrader.tar.xz")
# install=$pkgname.install

build() {
  cd "$srcdir"
 make
}

package() {
  cd "$srcdir"
  install -D -m755 downgrader "$pkgdir/usr/bin/downgrader"
} 
