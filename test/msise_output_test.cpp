#include "nrlmsise.hpp"
#include "space_weather.hpp"
#include <cstdio>
#include "datetime/datetime_write.hpp"
#include <array>

using namespace dso;

struct Input {
  TwoPartDateUTC utc;
  double x, y, z;
};

std::array<Input, 6> data = {{
{TwoPartDateUTC(60310, FractionalSeconds(7.320000000000e+04)), -5.806483047000e+06, 4.315083000000e+05, 5.063150222000e+06},
{TwoPartDateUTC(60310, FractionalSeconds(7.920000000000e+04)), -4.965743438000e+06, 5.860437573000e+06, 7.486841940000e+05},
{TwoPartDateUTC(60310, FractionalSeconds(8.520000000000e+04)), 3.000855080000e+05, 6.645091917000e+06, -3.913399634000e+06},
{TwoPartDateUTC(60312, FractionalSeconds(7.200000000000e+04)), -6.114670170000e+06, 9.296577950000e+05, 4.613265302000e+06},
{TwoPartDateUTC(60312, FractionalSeconds(7.800000000000e+04)), -4.665952093000e+06, 6.145992983000e+06, 1.303784630000e+05},
{TwoPartDateUTC(60312, FractionalSeconds(8.400000000000e+04)), 7.159074740000e+05, 6.290596922000e+06, -4.413039138000e+06}
}};

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s [CELESTRAK CSV FILE]\n", argv[0]);
    return 1;
  }

  TwoPartDateUTC d1(59580, FractionalSeconds(0)); // 2022/01/01
  TwoPartDateUTC d2(60795, FractionalSeconds(0)); // 2025/04/30

  /* collect space weather data fro CSV, in range [d1,d2) */
  const auto swdv = dso::load_celestrak_sw(argv[1], d1.utc2tt(), d2.utc2tt());

  /* initialize a Nrlmsise00 model */
  dso::Nrlmsise00 atmdens;

  /* and give it a data feed ... */
  atmdens.setup_data_hunter(swdv);

  char buf[64];
  double lon, lat, alt;
  double densities[9], temperatures[2];
  for (const auto &d : data) {
    Eigen::Vector3d itrf;
    itrf << d.x, d.y, d.z;
    const auto tt = d.utc.utc2tt();


    const double density = atmdens.density_debug(itrf, tt, lon, lat, alt, densities, temperatures);


    printf("%s %.9e %.9e %.9e ", to_char<YMDFormat::YYYYMMDD, HMSFormat::HHMMSSF>(d.utc, buf), lon, lat, alt);
    for (int i=0; i<9; i++) printf("%.12e ", densities[i]);
    for (int i=0; i<2; i++) printf("%.12e ", temperatures[i]);
    printf("%.15e\n", density);
  }

  return 0;
}
