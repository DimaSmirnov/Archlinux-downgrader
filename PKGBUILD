# Author: Dmitriy Smirnov <other@igus.lv>
pkgname=downgrader
pkgver=1.8.0
pkgrel=4
pkgdesc="Powerful packages downgrader for Archlinux. Works with libalpm, ARM and pacman logs"
arch=('i686' 'x86_64')
license=("GPL")
_gitname=Archlinux_downgrader

url="https://github.com/DimaSmirnov/Archlinux-downgrader"
source=("git://github.com/DimaSmirnov/Archlinux-downgrader.git")
  
# install=$pkgname.install

build() {
  cd src/$_gitname
  make
}

package() {
  cd src/$_gitname
  install -D -m755 downgrader "$pkgdir/usr/bin/downgrader"
}
md5sums=('SKIP')
