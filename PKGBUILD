# Developer: Dmitrij Smirnov <other@igus.lv>

pkgname=downgrader
pkgver=0.18
pkgrel=2
pkgdesc="Packages downgrader for Archlinux, powered by libalpm. Works with AUR, ARM and pacman logs"
arch=(i686 x86_64)
depends=('git' 'pacman' 'intltool')
license=("GPL3")
url="https://github.com/DimaSmirnov/Archlinux-downgrader"
_gitroot="git://github.com/DimaSmirnov/Archlinux-downgrader.git"
_gitname="Archlinux-downgrader"


build() {
  cd "$srcdir"
  msg "Connecting to the downgrader git repository..."
  if [ -d "$srcdir/$_gitname" ] ; then
    cd $_gitname && git pull origin
    msg "The local files are updated."
  else
    git clone $_gitroot 
  fi
  msg "GIT checkout done or server timeout"
  cd "$srcdir"
  rm -rf $_gitname-build
  git clone $_gitname $_gitname-build
  cd "$_gitname-build"
 ./configure --prefix=/usr
 make
}

package() {
  cd "$srcdir/$_gitname-build"
  make DESTDIR="$pkgdir/" install
}
