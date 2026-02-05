# Maintainer: Instellate
pkgname='proton-launcher'
pkgver='0.0.0'
pkgrel=1
pkgdesc='A program that manages a library of your games and launches them through proton'
arch=('x86_64')
url='https://github.com/Instellate/proton-launcher'
license=('GPL-3.0-only')
depends=('qt6-base' 'ki18n' 'kiconthemes' 'kcrash' 'kconfig' 'kcoreaddons' 'qqc2-desktop-style' 'kirigami' 'kirigami-addons')
makedepends=('gcc' 'cmake' 'git' 'extra-cmake-modules')
source=("${pkgname}::git+file://${PWD}")
sha256sums=('SKIP')

prepare() {
    mkdir "${srcdir}/build"
    cmake -B "${srcdir}/build" -S "${srcdir}/${pkgname}" -DCMAKE_BUILD_TYPE=Release
}

build() {
    cmake --build "${srcdir}/build"
}

package() {
    DESTDIR="${pkgdir}" cmake --install "${srcdir}/build"
    install -Dm644 "${srcdir}/${pkgname}/LICENSE" "${pkgdir}/usr/share/licenses/${pkgname}/LICENSE"
}
