if [ "$#" -ne 1 ]; then
    echo -e "Usage: \n$0 DIRECTORY\n\twhere DIRECTORY is a directory with sources for styling"
    exit 1
fi

if ! [ -x "$(command -v clang-format)" ]; then
  echo 'Error: clang-format is not installed. Please install clang-format with minimal version 3.8' >&2
  exit 1
fi

DIR=$1

FILES=`find $DIR -name "*.h" -or -name "*.cpp"`

for FILE in $FILES
do
	echo "Set style for $FILE"
	clang-format -i $FILE
done
