# Extend and dereference system test

This makes sure that when reusing assets from an existing zone, asset dependencies that are references can be overwritten to be non-references.

In this case:

- The zone `ZoneWithTechsetT6` contains the techniqueset `trivial_floatz_2992w610`
- The zone `ZoneWithMaterialT6` contains the material `test` and with a reference to its techniqueset `,trivial_floatz_2992w610`
- The final zone `CombinedZoneT6` is built while loading both of the other fastfile and is expected to contain both the material `test` with a reference to a (actual asset, not a reference) techniqueset `trivial_floatz_2992w610`
