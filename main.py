import ctypes
import os
from netCDF4 import Dataset
import numpy as np
import pandas as pd
import geopandas as gpd


def grid_bins(bins, weights, nbins, nrows, data, date):
    """
    Takes lists representing bins from level 3 binned MODIS data and calculates coordinate pairs for each bin
    :param bins: List of the id numbers for each bin
    :param weights: List of pixel weights used in calculating mean value for bin
    :param nbins: Number of bins in binned data set
    :param nrows: Number of rows in binned data set
    :param data: List of data values for each bin
    :param date: Date for binned file
    :return: Pandas dataframe containing latitudes, longitudes, and data values for each bin
    """
    _latlon = ctypes.CDLL('latlon.so')
    _latlon.gridBins.argtypes = (
        ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_float), ctypes.POINTER(ctypes.c_float),
        ctypes.c_int, ctypes.c_int, ctypes.POINTER(ctypes.c_float),
        ctypes.POINTER(ctypes.c_float), ctypes.POINTER(ctypes.c_float))

    bins_array_type = ctypes.c_int * nbins
    lats = (ctypes.c_float * nbins)()

    lons = (ctypes.c_float * nbins)()
    sst_array = (ctypes.c_float * nbins)(*data)
    sst_out = (ctypes.c_float * nbins)()
    weights_array = (ctypes.c_float * nbins)(*weights)
    _latlon.gridBins(bins_array_type(*bins), sst_array, weights_array, ctypes.c_int(nbins),
                     ctypes.c_int(nrows), lats, lons, sst_out)

    lats = list(lats)
    lons = list(lons)
    ssts = list(sst_out)
    df = pd.DataFrame(data={"Latitude": lats, "Longitude": lons, "SST": ssts, "Date": date})
    return df


def get_params(dataset):
    """
    Parses values from netCDF4 file for use in calculating lat/lon values for each bin
    :param dataset: netCDF4 object containing data
    :return:
    """
    nrows = len(dataset.groups['level-3_binned_data']['BinIndex'])
    binlist = np.array(dataset.groups['level-3_binned_data']['BinList'][:].tolist())
    sst = np.array(dataset.groups['level-3_binned_data']['chlor_a'][:].tolist())[:, 0]
    bins = binlist[:, 0]
    weights = binlist[:, 3]
    nbins = len(bins)
    return bins, weights, nbins, nrows, sst


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
    bins, weights, nbins, nrows, sst = get_params(dataset)
    df = grid_bins(bins.astype("int"), weights, nbins, nrows, sst, dataset.time_coverage_start)
    df = df[(df.Latitude >= latmin) & (df.Latitude <= latmax) &
            (df.Longitude >= lonmin) & (df.Longitude <= lonmax)]
    gdf = gpd.GeoDataFrame(
        df, geometry=gpd.points_from_xy(df.Longitude, df.Latitude))
    gdf.crs = "+proj=sinu +lon_0=0 +x_0=0 +y_0=0 +a=6371007.181 +b=6371007.181 +units=m +no_defs"
    return gdf


def map_files(directory, latmin, latmax, lonmin, lonmax):
    """
    Takes a directory of netCDF4 files of binned satellite data and creates shapefiles containing the data values
    for each bin
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
            gdf.to_file(cwd + "/out/" + year_month + "/" + outfile)
        except IOError as err:
            print("Error while attempting to save shapefile:", err)

        print("Finished %s", year_month)

map_files("/Users/christopherberglund/Desktop/testncdf", -90, 90, -180, 180)
