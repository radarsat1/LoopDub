#!/bin/sh

mkdir $1/loopdub
cp loopdub $1/loopdub/
cp Dotum10.raw $1/loopdub/
cp README $1/loopdub/
cp AUTHORS $1/loopdub/
cp LICENSE $1/loopdub/
cp TODO $1/loopdub/
cp INSTALL $1/loopdub/
cp -rf docs $1/loopdub/

# create launcher
echo "#!/bin/sh" >$2/loopdub
echo "cd $1/loopdub/; ./loopdub \$@" >>$2/loopdub
chmod +x $2/loopdub
