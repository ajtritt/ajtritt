import argparse
from datetime import datetime
import os
import sys
import warnings

desc = """
Merge multiple DOE COI spreadsheets and save into a new spreadsheet.
You will need to copy the merged spreadsheet and paste it into the Collaborators
tab of a bare COI template.

A list of key personnel will be printed to standard output in tab-delimited format.
You should be able to copy and paste this straight into the Senior-Key Personnel tab.
You will need to fill in the institutions of the Senior-Key Personnel.

The -e/--exclude flag can be used to provide a list of institutions to use for
filtering out collaborators. This isn't ChatGPT, so be sure to include all variants of
an institution e.g. Lawrence Berkeley Lab, LBNL, LBL, etc.

By default, all LBNL, NERSC, ESNet, JGI, ALS, and Molecular Foundry collaborators
are excluded.

You will need to have pandas and openpyxl install to run this script.
"""
parser = argparse.ArgumentParser(description=desc, formatter_class=argparse.RawDescriptionHelpFormatter)
parser.add_argument("out_xlsx", help="the path to save the merged spreadsheet to")
parser.add_argument("in_coi", nargs="+", help="the COIs to merge")
parser.add_argument("-e", "--exclude", help="a file of institutions to exclude, one line per institution", default=None)
parser.add_argument("--lbl", action='store_true', help='do not exclude LBNL collaborators', default=False)

args = parser.parse_args()

import pandas as pd

if args.out_xlsx in args.in_coi:
    print(f"The output spreadsheet name ({args.out_xlsx}) was also supplied as an input", file=sys.stderr)
    print("Exiting to prevent overwriting something", file=sys.stderr)
    exit(1)

if os.path.exists(args.out_xlsx):
    print(f"The output spreadsheet {args.out_xlsx} already exists", file=sys.stderr)
    print("Exiting to prevent overwriting something", file=sys.stderr)
    exit(1)

lbl_names = [
        'Lawrence Berkeley National Laboratory',
        'LBL',
        'LBNL',
        'Lawrence Berkeley National Lab',
        'Lawrence Berkeley Lab',
        'Lawrence Berkeley Laboratory',
        'Energy Sciences Network',
        'ESNet',
        'ESnet',
        'NERSC',
        'National Energy Research Scientific Computing Center',
        'Joint Genome Institute',
        'JGI',
        'Advanced Light Source',
        'ALS',
        'Molecular Foundry',
]
if args.lbl:
    ex = pd.Series()
else:
    ex = pd.Series(lbl_names)
if args.exclude is not None:
    ex = pd.concat([ex,
                    pd.read_csv(args.exclude, header=None).iloc[:, 0]]).drop_duplicates()

all_dfs = list()

for xls in args.in_coi:
    all_dfs.append(pd.read_excel(xls, sheet_name='Collaborators', skiprows=6))

final_df = pd.concat(all_dfs)

# filter out any weird garbage
filter_cols = list()
for i, col in enumerate(final_df.columns):
    # don't exclude on the basis of missing optional info
    if 'optional' in col:
        continue
    filter_cols.append(i)
mask = ~final_df.iloc[:, filter_cols].isna().any(axis=1)

final_df = final_df[mask]

def date_cleanup(val):
    try:
        return int(val)
    except:
        if isinstance(val, pd.Timestamp):
            return val.year
        raise ValueError(val)

# clean up years -- some people give full dates
final_df['Last Active \n(4-digit Year)'] = final_df['Last Active \n(4-digit Year)'].apply(date_cleanup)

last_year = datetime.now().year - 4
final_df = final_df[final_df['Last Active \n(4-digit Year)'] >= last_year]

if ex is not None:
    def keep_collab(row):
        val = row['Institution (Full Name)']
        try:
            for _ex in ex:
                if _ex in val:
                    return False
        except:
            person = row['First (Given) Name'] + " " + row['Last (Family) Name']
            warnings.warn(f"Excluding row {row.name} from {person} because I cannot check institution: '{val}'")
            return False
        return True
    final_df = final_df[final_df.apply(keep_collab, axis=1)]


institute_names = {
        'Pacific Northwest National Laboratory': 'PNNL',
        'Lawrence Berkeley National Laboratory': ['LBNL', 'LBL'],
        'Brookhaven National Laboratory': 'BNL',
        'Argonne National Laboratory': 'ANL',
        'Los Alamos National Laboratory': 'LANL',
        'Lawrence Livermore National Laboratory': 'LLNL',
        'National Renewable Energy Laboratory': 'NREL',
        'Oak Ridge National Laboratory': ['ORNL', 'Oakridge National Laboratory'],
        'University of California, Berkeley': ['UC Berkeley', 'UC-Berkeley'],
        'University of California, San Diego':['UC San Diego', 'UC-San Diego', 'UCSD'],
        'University of California, Santa Barbara': ['UC Santa Barbara', 'UC-Santa Barbara', 'UCSB'],
        'University of California, Irvine': ['UC Irvine', 'UC-Irvine'],
        'University of California, Davis': ['UC Davis', 'UC-Davis'],
        'University of California, San Francisco': ['UCSF', 'UC San Francisco', 'UC-San Francisco'],
        'European Organization for Nuclear Research': 'CERN',
        'Sandia National Laboratories ': ['Sandia National Laboratory', 'SNL'],
        'Nvidia': ['NVIDIA', 'Nvidia Inc'],
        'King Abdullah University of Science and Technology': 'KAUST',
        'University of Texas at Austin': ['UT Austin', 'UT', 'Texas', 'UT Austin Texas'],
        'University of Colorado Boulder': ['CU', 'CU Boulder', 'Colorado', 'Colorado Boulder'],
        'The University of Texas at San Antonio': 'UTSA',
}

tmp = dict()
for k, v in institute_names.items():
    if isinstance(v, list):
        for alternate in v:
            tmp[alternate] = k
    else:
        tmp[v] = k
institute_names = tmp

def norm_names(val):
    val = val.strip(", ")
    val = val.replace("/", " & ")
    val = val.replace("univ", "Univ")
    val = val.replace("Univ.", "University")
    val = val.replace("Univ ", "University")
    return institute_names.get(val, val)

final_df['Institution (Full Name)'] = final_df['Institution (Full Name)'].apply(norm_names)

final_df['Nature of Relationship'] = final_df['Nature of Relationship'].str.lower()

final_df.to_excel(args.out_xlsx, index=False)

key_personnel = final_df[['Last (Family) Name', 'First (Given) Name']]
key_personnel.drop_duplicates().to_csv(sys.stdout, index=False, header=False, sep='\t')
