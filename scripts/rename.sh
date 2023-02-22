#!/bin/bash
#
# Replace all instances of a name in a file with a new name.
# Copy a functionblock c/h file and rename all instances of the name.
#


# ${1} = base name
function camel_case_lower {
    echo ${1}
}

# ${1} = base name
function camel_case_upper {
    echo $(sed 's/[a-z]/\U&/' <<< ${1})
}

# ${1} = base name
function snake_case_lower {
    echo $(sed 's/\([A-Z]\{1,\}\)/_\L\1/g' <<< ${1})
}

# ${1} = base name
function snake_case_upper {
    echo $(sed 's/\([A-Z]\{1,\}\)/_\L\1/g;s/[a-z]/\U&/g' <<< ${1})
}

# ${1} = base name
function lower {
    echo $(sed -e 's/\(.*\)/\L\1/' <<< ${1})
}


# check numer of command line arguments is 3
if [ $# -ne 3 ]; then
    echo "Usage: $0 <file name> <old name> <new name>"
    echo "names must be in lower camel case e.g. binaryIoTypeA"
    exit 1
fi

file_name=$1
old_name=$2
new_name=$3

old_snake_lower=$(snake_case_lower ${old_name})
old_snake_upper=$(snake_case_upper ${old_name})
old_camel_lower=$(camel_case_lower ${old_name})
old_camel_upper=$(camel_case_upper ${old_name})
old_lower=$(lower ${old_name})

new_snake_lower=$(snake_case_lower ${new_name})
new_snake_upper=$(snake_case_upper ${new_name})
new_camel_lower=$(camel_case_lower ${new_name})
new_camel_upper=$(camel_case_upper ${new_name})
new_lower=$(lower ${new_name})

echo "replacing in ${file_name}:"

echo "lower: ${old_lower} -> ${new_lower}"
echo "snake lower: ${old_snake_lower} -> ${new_snake_lower}"
echo "snake upper: ${old_snake_upper} -> ${new_snake_upper}"
echo "camel lower: ${old_camel_lower} -> ${new_camel_lower}"
echo "camel upper: ${old_camel_upper} -> ${new_camel_upper}"

sed -i -e s/${old_lower}/${new_lower}/g ${file_name}
sed -i -e s/${old_snake_lower}/${new_snake_lower}/g ${file_name}
sed -i -e s/${old_snake_upper}/${new_snake_upper}/g ${file_name}
sed -i -e s/${old_camel_lower}/${new_camel_lower}/g ${file_name}
sed -i -e s/${old_camel_upper}/${new_camel_upper}/g ${file_name}
