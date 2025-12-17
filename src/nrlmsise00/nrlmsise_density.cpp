#include "geodesy/transformations.hpp"
#include "nrlmsise.hpp"
#include <cstring>

double dso::Nrlmsise00::density(Eigen::Vector3d rsat, const dso::MjdEpoch &tt,
                                const dso::Msise00Data &data)
{

  /* cartesian to geodetic satellite position */
  const auto llh = dso::cartesian2geodetic<dso::ellipsoid::grs80>(
      dso::CartesianCrdConstView(rsat));

  /* temperatures and densities */
  double densities[9];
  double temperatures[2];

  /* */
  return gts7(tt, llh, data.f107A, data.f107, data.ap, densities, temperatures);
}

double dso::Nrlmsise00::density_debug(Eigen::Vector3d rsat, const dso::MjdEpoch &tt,
                                const dso::Msise00Data &data,
                                double &lon, double &lat, double &alt,
                                double *densities_out,
                                double *temperatures_out
                                )
{

  /* cartesian to geodetic satellite position */
  const auto llh = dso::cartesian2geodetic<dso::ellipsoid::grs80>(
      dso::CartesianCrdConstView(rsat));

  /* temperatures and densities */
  double densities[9];
  double temperatures[2];

  /* */
  double density = gts7(tt, llh, data.f107A, data.f107, data.ap, densities, temperatures);

  printf("\tData f107A: %.7e f107: %.7e", data.f107A, data.f107);
  for (int k=0;k<7;k++) printf(" %.7e", data.ap[k]);
  printf("\n");

  lon = llh.lon();
  lat = llh.lat();
  alt = llh.hgt();
  std::memcpy((void *)densities_out, (void *)densities, sizeof(double)*9);
  std::memcpy((void *)temperatures_out, (void *)temperatures, sizeof(double)*2);
  return density;
}
