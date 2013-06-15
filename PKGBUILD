# Author: Dmitriy Smirnov <other@igus.lv>
pkgname=downgrader
pkgver=1.5.6
pkgrel=2
pkgdesc="Powerful packages downgrader for Archlinux. Works with libalpm, ARM and pacman logs"
arch=('any')
depends=('pacman' 'intltool' 'sudo')
license=("GPL")
url="https://github.com/DimaSmirnov/Archlinux-downgrader"
source=("http://www.igus.lv/downgrader/downgrader.tar.xz")

build() {
  cd "$srcdir"
 make
}
package() {
  echo ">>>>> Now, for correct work I need to clear your pacman log file. Please confirm"
  sudo rm /var/log/pacman.log
  sudo touch /var/log/pacman.log
  cd "$srcdir"
  install -D downgrader "$pkgdir/usr/bin/downgrader"
}
