" BEGIN code for vim-plug
" begin automatically install vim-plug
let data_dir = has('nvim') ? stdpath('data') . '/site' : '~/.vim'
if empty(glob(data_dir . '/autoload/plug.vim'))
  silent execute '!curl -fLo '.data_dir.'/autoload/plug.vim --create-dirs  https://raw.githubusercontent.com/junegunn/vim-plug/master/plug.vim'
  autocmd VimEnter * PlugInstall --sync | source $MYVIMRC
endif
" end autmatically install vim-plug
"
" Plugins will be downloaded under the specified directory.
call plug#begin(has('nvim') ? stdpath('data') . '/plugged' : '~/.vim/plugged')

" Declare the list of plugins.
Plug 'tpope/vim-sensible'
Plug 'junegunn/seoul256.vim'
Plug 'morhetz/gruvbox'

" List ends here. Plugins become visible to Vim after this call.
call plug#end()
" END code for vim-plug

set nocompatible
set cursorline
syntax on
set number
set ts=4
set ruler
set nowrap
set listchars=eol:$,tab:>~,trail:~,extends:>,precedes:<,nbsp:-
set scrolljump=10
set hlsearch
set laststatus=2
" BEGIN no more holding down keys
noremap jj <nop>
noremap kk <nop>
noremap ll <nop>
noremap hh <nop>
set timeoutlen=50
autocmd InsertEnter * set timeoutlen=1000
autocmd InsertLeave * set timeoutlen=50
" END no more holding down keys

set background=dark " this line will make everything look like easter egg puke

colorscheme gruvbox
au BufRead,BufNewFile *.cu set filetype=cuda
au BufRead,BufNewFile *.upc set filetype=c
au! Syntax cuda source ~/.vim/syntax/cuda.vim
au FileType c,cpp,java,cuda,perl set mps+==:;
au FileType cpp,java set mps+=<:>
au FileType json set ts=2
hi Comment ctermfg=grey
autocmd BufNewFile,BufRead *.json set ft=javascript
set expandtab
nmap <F1> <nop>
"disable arrow keys
map <up> <nop>
map <down> <nop>
map <left> <nop>
map <right> <nop>
noremap [ }
noremap { [
noremap ] {
noremap } ]
"nmap h <nop>
"nmap j <nop>
"nmap k <nop>
"nmap l <nop>
"map <esc> <nop>
"leave insert mode by jj
imap jj <ESC>
nmap rw vepbyw
nmap rl Vpyy
map <C-c> :noh<CR>
"set et
"map d<Tab> d4h
"set mouse=a
"set tw=70
"set ts=4

highlight ExtraWhitespace ctermbg=red guibg=red
match ExtraWhitespace /\s\+$/

" automatically remove trailing whitespace in Python files
autocmd BufWritePre *.c,*.py,*.rst,*.json,*.cpp,*.yml,*.yaml,*.tex %s/\s\+$//e
set backspace=indent,eol,start
