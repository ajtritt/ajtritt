export PATH=$PATH:$HOME/bin

alias tok='2fa nersc | pbcopy'

function tokpw() {
    read -r -p "Enter the passcode ${user}: " -s pw
    _____B=`echo U2FsdGVkX18uZuliD0z+/IPn1CXqJ2iixBMRM9Gnwr5NtgBPL5AwhDdRfIwPonvY | openssl aes-256-cbc -pbkdf2 -d -a -pass pass:$pw 2> /dev/null`
    if [ $? -ne 0 ]; then 
        echo "Inccorrect passcode" >&2
        return 1
    fi
    echo `cat $_____B | openssl aes-256-cbc -pbkdf2 -d -a -pass pass:$pw``2fa nersc` | pbcopy
    printf "Success!\n"
}
