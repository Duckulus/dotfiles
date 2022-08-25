#check for root
if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

homedir=/home/$(logname)
echo Found Homedir $homedir
# install zsh
echo installing zsh
pacman -S zsh
sh -c "$(curl -fsSL https://raw.github.com/ohmyzsh/ohmyzsh/master/tools/install.sh)"
folder=$homedir/.oh-my-zsh/custom/themes/powerlevel10k
if [ ! -d ${folder} ]  ; then
  echo cloning 
  git clone --depth=1 https://github.com/romkatv/powerlevel10k.git ${folder} 
else
  echo p10k already installed
fi

mv $homedir/.zshrc $homedir/.zshrc.bak
ln -s $PWD/.zshrc $homedir/.zshrc
mv $homedir/.p10k.zsh $homedir/.p10k.zsh.bak
ln -s $PWD/.p10k.zsh $homedir/.p10k.zsh

#set wallpaper
mv $homedir/.wallpaper $homedir/.wallpaper.bak
ln -s $PWD/.wallpaper $homedir/.wallpaper

mv $homedir/.fehbg $homedir/.fehbg.bak
ln -s $PWD/.fehbg $homedir/.fehbg

feh --bg-scale $homedir/.wallpaper/wallpaper.png
