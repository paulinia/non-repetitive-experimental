#!/bin/bash

if [ "$#" -lt 2 ]; then
    echo "Usage: $0 <list_file> <out_name>"
    exit 1
fi

list_file="$1"
out_name="$2"

if [ ! -x "./entropy_compression" ] && [ -f "entropy_compression.cpp" ]; then
    g++ -O3 entropy_compression.cpp -o entropy_compression
fi

i=1
while read -r script params || [ -n "$script" ]; do
    # Skip empty lines and comments
    [[ -z "$script" || "$script" == \#* ]] && continue

    ext=""
    base="$script"

    if [[ "$script" == *.py ]]; then
        ext="py"
        base="${script%.py}"
    elif [[ "$script" == *.cpp ]]; then
        ext="cpp"
        base="${script%.cpp}"
    else
        # No extension provided: search for matching file and guess extension
        if [ -f "${script}.py" ]; then
            ext="py"
        elif [ -f "${script}.cpp" ] || [ -x "./${script}" ]; then
            ext="cpp"
        fi
    fi

    in_file="${out_name}${i}.in"
    log_file="log${i}.out"
    col_file="col${i}.out"
    svg_file="${out_name}${i}.svg"

    if [ "$ext" == "py" ]; then
        echo "$params" | python3 "${base}.py" > "$in_file"
    elif [ "$ext" == "cpp" ]; then
        if [ ! -x "./${base}" ] && [ -f "${base}.cpp" ]; then
            g++ -O3 "${base}.cpp" -o "${base}"
        fi
        echo "$params" | ./"${base}" > "$in_file"
    else
        echo "Error: Could not find generator '$script' (.cpp or .py)" >&2
        exit 1
    fi

    time ./entropy_compression < "$in_file" > "$log_file"
    tail -n 2 "$log_file" > "$col_file"
    python3 visualise.py "$in_file" "$col_file" "$svg_file"

    i=$((i + 1))
done < "$list_file"
