# Applies renaming within all of the rapidjson source files to add a cereal prefix
find ./../include/cereal/external/rapidjson/ -type f -name \*.h -exec sed -i "s/RAPIDJSON_/CEREAL_RAPIDJSON_/g" {} \;
echo "Remember to backport any cereal specific changes not in this version of RapidJSON!"
echo "See https://github.com/USCiLab/cereal/commits/develop/include/cereal/external/rapidjson"
