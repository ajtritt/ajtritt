
cd `dirname $0`

cp -r .gitconfig .inputrc .tmux .tmux.conf .vimrc $HOME

bash_config="$HOME/.bash_config"
if [ ! -d "$bash_config" ]; then
    echo "Making $bash_config"
    mkdir $bash_config
fi

cp bash/* ~/.bash_config/

tmpfile=$(mktemp /tmp/ajtritt.XXXXXX)

echo "for sh in $bash_config/*; do source \$sh; done" > $tmpfile
cat $HOME/.bash_profile >> $tmpfile
echo "" >> $tmpfile
cp $tmpfile $HOME/.bash_profile
rm $tmpfile

cd -
