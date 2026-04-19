Dokany import integration
========================

This directory vendors the minimum Dokany surface needed for Cxbx-Reloaded to
compile against the upstream user-mode API without building the Dokany driver,
runtime DLLs, installer, or samples in this tree.

Upstream source:
- Repository: https://github.com/dokan-dev/dokany
- Snapshot: `767da4ba1522c9221d7b70f98446096faffcbc8b`

Vendored files:
- `dokan/dokan.h`
- `dokan/dokanc.h`
- `dokan/fileinfo.h`
- `dokan/dokan.def`
- `sys/public.h`
- `LICENSE.LGPL.txt`
- `LICENSE.MIT.txt`

Build behavior:
- `projects/dokany/CMakeLists.txt` generates `dokan2.lib` from `dokan.def`
  with the MSVC librarian.
- End users are expected to install the signed Dokany runtime and driver
