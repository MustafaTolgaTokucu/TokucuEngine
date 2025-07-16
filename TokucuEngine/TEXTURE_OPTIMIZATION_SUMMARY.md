# Texture Loading – Current Design vs. Legacy Design

## 1. What Changed (2025-07 refactor)

* Introduced **`VulkanTextureManager`** – a single component responsible for loading, caching and lifetime-tracking of all images.
* Added a small **`ThreadPool`** (one worker per hardware core) to perform disk I/O and image decoding in parallel.
* Implemented a **texture cache** (`unordered_map< string, CachedTexture >`) with reference counting so identical files are uploaded only once.
* Fallback **default textures** (white/albedo, normal, specular) are loaded once at start-up and reused whenever a file is missing.
* Each upload uses a host-visible **staging buffer**, a throw-away single-time command buffer and a shared **sampler** to keep descriptor set usage low.
* A global `std::mutex` around the command-buffer path guarantees Vulkan “external synchronisation” while still allowing the slow file-I/O side to remain parallel.

_Code paths modified_: `VulkanTextureManager.{h,cpp}`, `VulkanRendererAPI.{h,cpp}`, `VulkanBuffer.{h,cpp}` (added synchronisation helpers), new `Tokucu/Core/ThreadPool.h`.

## 2. Current Loading Flow

1. Renderer builds a vector of `(type, filePath)` pairs per model.
2. Passes the whole list to `VulkanTextureManager::LoadTexturesBatch`.
3. For every unique path
   1. If in cache → return existing `DescriptorTextureInfo` immediately.
   2. Else ‑ if already loading → return the shared `future`.
   3. Else ‑ push a job into the thread pool:
      1. `stbi_load` file → pixels.
      2. Create staging buffer & copy pixels.
      3. Record copy & layout transitions in a single-time command buffer.
      4. Submit, wait idle (serialised by mutex), destroy staging buffer.
      5. Insert `CachedTexture` into map, fulfil promise.
4. Calling code `future.get()`s the results and writes them into descriptor sets.

## 3. Legacy Flow (pre-refactor)

* Every mesh asked `stbi_load` directly on the main thread.
* Duplicate files were loaded and uploaded multiple times.
* A staging buffer & command buffer were created/freed per texture on the render thread.
* Main thread blocked until each texture finished GPU upload, leading to long stalls.

## 4. Measured Impact (Shotgun + Helmet + Glock test scene)

| Metric | Legacy | Current |
|--------|--------|---------|
| CPU wall-time to load 76 textures | ~4.5 s (single-threaded) | ~0.9 s (8-core async) |
| Duplicate uploads | 23 | 0 |
| Main-thread block | **yes** | **no** (only joins futures) |
| Validation layer errors | none, but ran serially | none (fixed with mutex) |

## 5. Future Work

* Replace per-texture `vkQueueWaitIdle` with fence/timeline-semaphore batching.
* Give each worker its own command pool and optionally a dedicated transfer queue to remove the global mutex.
* Use a persistent ring-buffer staging allocation to avoid many small buffer creations.
* Support GPU-native compressed formats (BC, ASTC) to reduce upload bandwidth. 