# This is an example PKGBUILD file. Use this as a start to creating your own,
# and remove these comments. For more information, see 'man PKGBUILD'.
# NOTE: Please fill out the license field for your package! If it is unknown,
# then please put 'unknown'.
#Maintainer: Abouzakaria <zkariakov@gmail.com>
pkgname=qobbar
pkgver=0.2.1
pkgrel=1
epoch=
pkgdesc=""
arch=( 'x86_64')
url=""
license=('GPL')
groups=()
depends=('libx11' 'libxcomposite'  'libxdamage' 'qt5-x11extras' 'qt5-base' )
makedepends=()
checkdepends=()
optdepends=()
provides=()
conflicts=()
replaces=()
backup=()
options=()
install=
changelog=
source=("https://github.com/zakariakov/qobbar/archive/0.2.1.tar.gz"  )
      
noextract=()
md5sums=("SKIP")
validpgpkeys=()

prepare() {
	cd "$pkgname-$pkgver"
	
}

build() {
	cd "$srcdir/$pkgname-$pkgver"
	qmake "qobbar.pro" \
    PREFIX=/usr \
    QMAKE_CFLAGS_RELEASE="${CFLAGS}"\
    QMAKE_CXXFLAGS_RELEASE="${CXXFLAGS}"
	make
}

check() {
	cd "$pkgname-$pkgver"
	make -k check
}

package() {
	cd "$pkgname-$pkgver"
	make DESTDIR="$pkgdir/" install

    
    make INSTALL_ROOT="${pkgdir}"/ install || return 1
    find "$pkgdir" -type d -print0 | xargs -0 chmod -R 755
    find "$pkgdir" -type f -print0 | xargs -0 chmod -R 644
    chmod 755 "$pkgdir/usr/bin/qobbar"

}
