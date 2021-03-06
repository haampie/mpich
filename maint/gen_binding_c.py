##
## Copyright (C) by Argonne National Laboratory
##     See COPYRIGHT in top-level directory
##

from local_python import MPI_API_Global as G
from local_python.mpi_api import *
from local_python.binding_c import *
from local_python import RE
import glob
import os

def main():
    binding_dir = "src/binding"

    # -- Loading Standard APIs --
    if os.path.exists("%s/apis.json" % binding_dir):
        print("Loading %s/apis.json ..." % binding_dir)
        load_mpi_json("%s/apis.json" % binding_dir)
    else:
        print("Loading %s/mpi_standard_api.txt ..." % binding_dir)
        load_mpi_api("%s/mpi_standard_api.txt" % binding_dir)

    print("Loading %s/apis_mapping.txt ..." % binding_dir)
    load_mpi_mapping("%s/apis_mapping.txt" % binding_dir)
    print("Loading %s/custom_mapping.txt ..." % binding_dir)
    load_mpi_mapping("%s/custom_mapping.txt" % binding_dir)

    # -- Loading MPICH APIs --
    binding_dir = "src/binding/c"

    api_files = glob.glob("%s/*_api.txt" % binding_dir)
    for f in api_files:
        if RE.match(r'.*\/(\w+)_api.txt', f):
            # The name in eg pt2pt_api.txt indicates the output folder.
            # Only the api functions with output folder will get generated.
            # This allows simple control of what functions to generate.
            print("Loading %s ..." % f)
            load_mpi_api(f, RE.m.group(1))

    # -- Generating code --
    func_list = [f for f in G.FUNCS.values() if 'dir' in f]
    func_list.sort(key = lambda f: f['dir'])
    for func in func_list:
        if 'not_implemented' in func:
            print("  skip %s (not_implemented)" % func['name'])
            pass
        else:
            G.out = []
            G.err_codes = {}
            mapping = G.MAPS['SMALL_C_KIND_MAP']

            # dumps the code to G.out array
            dump_mpi_c(func, mapping)

            file_path = get_func_file_path(func, binding_dir)
            dump_c_file(file_path, G.out)

            # add to mpi_sources for dump_Makefile_mk()
            G.mpi_sources.append(file_path)

    dump_Makefile_mk(binding_dir + "/Makefile.mk")
    dump_mpir_impl_h("src/include/mpir_impl.h")
    dump_errnames_txt(binding_dir + "/errnames.txt")

# ---------------------------------------------------------
if __name__ == "__main__":
    main()
