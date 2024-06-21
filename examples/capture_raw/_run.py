import sys
from pathlib import Path

# import python folder
cwd = Path(__file__).parent.absolute()
root = Path(__file__).parents[2].absolute()
python_path = root / "python"
sys.path.append(str(python_path))
from run_xscope_bin import run

# run
xe_file = cwd / "bin" / "capture_raw.xe"
assert(xe_file.exists())
run(xe_file, return_stdout=False)
