function findgrep ()
{
    local wc=${1:?"Missing wildcard pattern"};
    local pat=${2:?"Missing pattern to grep for"};
    find . -name "$wc" -exec grep -Hn $pat {} \;
}
