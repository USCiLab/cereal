// swift-tools-version:5.5
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "cereal",
    products: [
        // Products define the executables and libraries a package produces, and make them visible to other packages.
        .library(
            name: "cereal",
            targets: ["cereal"]),
    ],
    dependencies: [
        // Dependencies declare other packages that this package depends on.
        // .package(url: /* package url */, from: "1.0.0"),
    ],
    targets: [
        // Targets are the basic building blocks of a package. A target can define a module or a test suite.
        // Targets can depend on other targets in this package, and on products in packages this package depends on.
        .target(
            name: "cereal",
            dependencies: [],
            path: ".",
            exclude: ["CMakeLists.txt",
                      "Config.cmake.in",
                      "LICENSE",
                      "README.md",
                      "appveyor.yml",
                      "doc",
                      "sandbox",
                      "scripts",
                      "unittests",
                      "include/cereal/external/rapidjson/LICENSE",
                      "include/cereal/external/rapidjson/msinttypes/LICENSE",
                      "include/cereal/external/rapidxml/manual.html",
                      "include/cereal/external/LICENSE",
                      "include/cereal/external/rapidxml/license.txt"],
            sources: ["spm_dummy.cpp"],
            publicHeadersPath: "include"),
    ],
    cxxLanguageStandard: .cxx14
)
