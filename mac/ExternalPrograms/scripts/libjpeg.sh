# ------------------
#     libjpeg
# ------------------
# $Id: libjpeg.sh 1902 2007-02-04 22:27:47Z ippei $
# Copyright (c) 2007, Ippei Ukai


# prepare

# export REPOSITORYDIR="/PATH2HUGIN/mac/ExternalPrograms/repository" \
# ARCHS="ppc i386" \
#  ppcTARGET="powerpc-apple-darwin7" \
#  ppcMACSDKDIR="/Developer/SDKs/MacOSX10.3.9.sdk" \
#  ppcONLYARG="-mcpu=G3 -mtune=G4 -mmacosx-version-min=10.3" \
#  i386TARGET="i386-apple-darwin8" \
#  i386MACSDKDIR="/Developer/SDKs/MacOSX10.4u.sdk" \
#  i386ONLYARG="-march=prescott -mtune=pentium-m -ftree-vectorize -mmacosx-version-min=10.4" \
#  OTHERARGs="";


# init

let NUMARCH="0"

for i in $ARCHS
do
  NUMARCH=$(($NUMARCH + 1))
done

mkdir -p "$REPOSITORYDIR/bin";
mkdir -p "$REPOSITORYDIR/lib";
mkdir -p "$REPOSITORYDIR/include";


# compile

# update some of libtool stuff: config.guess and config.sub -- location has changed between 10.5 and 10.6
path="/usr/share/libtool";
[ -d "${path}/config" ] && path="${path}/config";
cp -v ${path}/config.{guess,sub} ./;

for ARCH in $ARCHS
do

 mkdir -p "$REPOSITORYDIR/arch/$ARCH/bin";
 mkdir -p "$REPOSITORYDIR/arch/$ARCH/lib";
 mkdir -p "$REPOSITORYDIR/arch/$ARCH/include";

 ARCHARGs=""
 MACSDKDIR=""

 if [ $ARCH = "i386" -o $ARCH = "i686" ]
 then
  TARGET=$i386TARGET
  MACSDKDIR=$i386MACSDKDIR
  ARCHARGs="$i386ONLYARG"
  CC=$i386CC
  CXX=$i386CXX
 elif [ $ARCH = "ppc" -o $ARCH = "ppc750" -o $ARCH = "ppc7400" ]
 then
  TARGET=$ppcTARGET
  MACSDKDIR=$ppcMACSDKDIR
  ARCHARGs="$ppcONLYARG"
  CC=$ppcCC
  CXX=$ppcCXX
 elif [ $ARCH = "ppc64" -o $ARCH = "ppc970" ]
 then
  TARGET=$ppc64TARGET
  MACSDKDIR=$ppc64MACSDKDIR
  ARCHARGs="$ppc64ONLYARG"
  CC=$ppc64CC
  CXX=$ppc64CXX
 elif [ $ARCH = "x86_64" ]
 then
  TARGET=$x64TARGET
  MACSDKDIR=$x64MACSDKDIR
  ARCHARGs="$x64ONLYARG"
  CC=$x64CC
  CXX=$x64CXX
 fi

 env \
  CC=$CC CXX=$CXX \
  CFLAGS="-isysroot $MACSDKDIR -arch $ARCH $ARCHARGs $OTHERARGs -dead_strip" \
  LDFLAGS="-L$REPOSITORYDIR/lib -isysroot $MACSDKDIR -arch $ARCH -dead_strip" \
  ./configure --prefix="$REPOSITORYDIR" --disable-dependency-tracking \
  --host="$TARGET" --exec-prefix=$REPOSITORYDIR/arch/$ARCH \
  --disable-shared --enable-static;

 make clean;
 make
 make install-lib;

 # the old config-make stuff do not create shared library well. Best do it by hand.
 rm "libjpeg.62.0.0.dylib";
 $CC -isysroot $MACSDKDIR -arch $ARCH $ARCHARGs $OTHERARGs -dead_strip \
  -dynamiclib -flat_namespace -undefined suppress \
  -lmx -shared-libgcc -current_version 62.0.0 -compatibility_version 62.0.0\
  -install_name "$REPOSITORYDIR/lib/libjpeg.62.dylib" -o libjpeg.62.0.0.dylib \
  jcomapi.o jutils.o jerror.o jmemmgr.o jmemnobs.o \
  jcapimin.o jcapistd.o jctrans.o jcparam.o jdatadst.o jcinit.o \
  jcmaster.o jcmarker.o jcmainct.o jcprepct.o jccoefct.o jccolor.o \
  jcsample.o jchuff.o jcphuff.o jcdctmgr.o jfdctfst.o jfdctflt.o \
  jfdctint.o \
  jdapimin.o jdapistd.o jdtrans.o jdatasrc.o jdmaster.o \
  jdinput.o jdmarker.o jdhuff.o jdphuff.o jdmainct.o jdcoefct.o \
  jdpostct.o jddctmgr.o jidctfst.o jidctflt.o jidctint.o jidctred.o \
  jdsample.o jdcolor.o jquant1.o jquant2.o jdmerge.o;
 install "libjpeg.62.0.0.dylib" "$REPOSITORYDIR/arch/$ARCH/lib";

done


# merge libjpeg

for liba in lib/libjpeg.a lib/libjpeg.62.0.0.dylib
do

 if [ $NUMARCH -eq 1 ] ; then
   mv "$REPOSITORYDIR/arch/$ARCHS/$liba" "$REPOSITORYDIR/$liba";
   #Power programming: if filename ends in "a" then ...
   [ ${liba##*.} = a ] && ranlib "$REPOSITORYDIR/$liba";
   continue
 fi

 LIPOARGs=""
 
 for ARCH in $ARCHS
 do
   LIPOARGs="$LIPOARGs $REPOSITORYDIR/arch/$ARCH/$liba"
 done

 lipo $LIPOARGs -create -output "$REPOSITORYDIR/$liba";
 #Power programming: if filename ends in "a" then ...
 [ ${liba##*.} = a ] && ranlib "$REPOSITORYDIR/$liba";
 
done


if [ -f "$REPOSITORYDIR/lib/libjpeg.62.0.0.dylib" ]
then
 ln -sfn "libjpeg.62.0.0.dylib" "$REPOSITORYDIR/lib/libjpeg.62.dylib";
 ln -sfn "libjpeg.62.0.0.dylib" "$REPOSITORYDIR/lib/libjpeg.dylib";
fi
