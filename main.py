import ctypes
import os
from netCDF4 import Dataset
import numpy as np
import pandas as pd
import geopandas as gpd


def boa(total_bins, nrows, fill_value, bins, data, weights, date, chlor_a=False):
    """
    Performs the Belkin-O'Reilly front detection algorithm on the provided bins
    :param total_bins: total number of bins in the binning scheme
    :param nrows: number of rows in the binning scheme
    :param fill_value: value to fill empty bins with
    :param bins: bin numbers for all data containing bins
    :param data: weighted sum of the data for each bin
    :param weights: weights used to calculate weighted sum for each bin
    :param date: date of the temporal bin
    :param chlor_a: if the data is chlorophyll a concentration, the natural lograithm of the data will be used
    in edge detection default is false
    :return: pandas dataframe containing bin values of each bin resulting from edge detection algorithm
    """
    _boa = ctypes.CDLL('boa.so')
    _boa.boa.argtypes = (ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_double, ctypes.POINTER(ctypes.c_int),
                         ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double),
                         ctypes.POINTER(ctypes.c_double), ctypes.POINTER(ctypes.c_double),
                         ctypes.POINTER(ctypes.c_double), ctypes.c_bool)

    bins_array_type = ctypes.c_int * len(bins)
    lats = (ctypes.c_double * total_bins)()

    lons = (ctypes.c_double * total_bins)()
    data_array = (ctypes.c_double * len(data))(*data)
    data_out = (ctypes.c_double * total_bins)()
    weights_array = (ctypes.c_double * len(bins))(*weights)
    _boa.boa(total_bins, len(bins), nrows, fill_value, bins_array_type(*bins), data_array, weights_array, lats, lons,
             data_out, 0)
    lats = list(lats)
    lons = list(lons)
    final_data = list(data_out)
    df = pd.DataFrame(
        data={"Latitude": lats, "Longitude": lons, "Data": final_data, "Date": np.repeat(date, len(lats))})
    return df


def get_params(dataset, data_str):
    """
    Parses values from netCDF4 file for use in Belkin-O'Reilly algorithm
    :param dataset: netCDF4 object containing data
    :param data_str: string key for data in netCDF4 Dataset object
    :return: the total number of bins in binning scheme, the number of rows, list of all data containing bins,
    data value for each bin, weight value for each bin
    """
    total_bins = np.array(dataset.groups["level-3_binned_data"]["BinIndex"][:].tolist())[:, 3].sum()
    nrows = len(dataset.groups["level-3_binned_data"]["BinIndex"])
    binlist = np.array(dataset["level-3_binned_data"]["BinList"][:].tolist())
    bins = binlist[:, 0].astype("int")
    weights = binlist[:, 3]
    data = np.array(dataset.groups["level-3_binned_data"][data_str][:].tolist())[:, 0]
    date = dataset.time_coverage_start

    return total_bins, nrows, bins, data, weights, date


def map_bins(dataset, latmin, latmax, lonmin, lonmax):
    """
    Takes a netCDF4 dataset of binned satellite data and creates a geodataframe with coordinates and bin data values
    :param dataset: netCDF4 dataset containing bins and data values
    :param latmin: minimum latitude to include in output
    :param latmax: maximum latitude to include in output
    :param lonmin: minimum longitude to include in output
    :param lonmax: maximum longitude to include in output
    :return: geodataframe containing latitudes, longitudes, and data values of all bins within given extent
    """
    total_bins, nrows, bins, data, weights, date = get_params(dataset, "chlor_a")
    df = boa(total_bins, nrows, -999.0, bins, data, weights, date, True)
    print("Cropping")
    df = df[(df.Latitude >= latmin) & (df.Latitude <= latmax) &
            (df.Longitude >= lonmin) & (df.Longitude <= lonmax)]
    df = df[df['Data'] > -999]
    df = df[df['Data'] < 2]
    df.to_csv('/Users/christopherberglund/Desktop/test.csv')
    return df


def map_files(directory, latmin, latmax, lonmin, lonmax):
    """
    Takes a directory of netCDF4 files of binned satellite data and creates shapefiles containing the values from
    the edge detection algorithm for each bin
    :param directory: directory path containing all netCDF4 files
    :param latmin: minimum latitude to include in output
    :param latmax: maximum latitude to include in output
    :param lonmin: minimum longitude to include in output
    :param lonmax: maximum longitude to include in output
    """
    cwd = os.getcwd()
    if not os.path.exists(cwd + "/out"):
        os.makedirs(cwd + "/out")
    for file in os.listdir(directory):
        if not file.endswith(".nc"):
            continue
        dataset = Dataset(directory + "/" + file)
        gdf = map_bins(dataset, latmin, latmax, lonmin, lonmax)
        year_month = dataset.time_coverage_start[:7]
        outfile = year_month + '_sst.shp'
        dataset.close()
        if not os.path.exists(cwd + "/out/" + year_month):
            os.makedirs(cwd + "/out/" + year_month)
        try:
            print('test')
            #gdf.to_file(cwd + "/out/" + year_month + "/" + outfile)
        except IOError as err:
            print("Error while attempting to save shapefile:", err)

        print("Finished %s", year_month)


map_files("/Users/christopherberglund/Desktop/testncdf", 20, 80, -180, -120)
