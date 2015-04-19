#!/bin/sh
# This is the simple pbuilder(1) hook that is invoked after unpacking the build
# chroot, what it does is:
#
# 1) creates /tmp/repository
# 2) hardlinks everything from /var/cache/apt/archives/*.deb to /tmp/repository
# 3) dpkg-scanpackages is /tmp/repository
# 4) add /tmp/repository to /etc/apt/sources.list
# 5) apt-get update
#
# The goal is to have build dependencies previously built with pbuilder and
# manually copied to APTCACHE of that pbuilder available within a debian
# repository

echo "Preparing local debian repository with contents of APTCACHE"

mkdir -p /tmp/repository
ln /var/cache/apt/archives/*.deb /tmp/repository
(cd /tmp/repository && dpkg-scanpackages . /dev/null >Packages)
cat >>/etc/apt/sources.list <<END

deb file:///tmp/repository ./
END

apt-get update

echo "DONE: Preparing local debian repository with contents of APTCACHE"
