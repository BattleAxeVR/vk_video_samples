/*
* Copyright 2023 NVIDIA Corporation.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "VkVideoEncoder/VkEncoderConfigAV1.h"

#define READ_PARAM(i, param, type) {                            \
    int32_t data = 0;                                           \
    if ((++i >= argc) || (sscanf(argv[i], "%d", &data) != 1)) { \
        fprintf(stderr, "invalid parameter");                   \
        return -1;                                              \
    } else {                                                    \
        param = (type)data;                                     \
    }                                                           \
}

int EncoderConfigAV1::DoParseArguments(int argc, char* argv[])
{
    // No validation of command line options.  So, all options must be valid and
    // values with in the limits of vulkan and av1 specification
    std::vector<std::string> args(argv, argv + argc);
    for (int32_t i = 0; i < argc; i++) {

        if (args[i] == "--tiles") {
            enableTiles = true;

            if (((i + 1) < argc) && (args[i + 1] == "--params")) {
                ++i;
                customTileConfig = true;
                READ_PARAM(i, tileConfig.flags.uniform_tile_spacing_flag, bool);
                READ_PARAM(i, tileConfig.TileCols, uint8_t);
                if (!tileConfig.flags.uniform_tile_spacing_flag) {
                    for (int32_t j = 0; j < tileConfig.TileCols; j++) {
                        READ_PARAM(i, tileWidthInSbsMinus1[j], uint16_t);
                    }
                }
                READ_PARAM(i, tileConfig.TileRows, uint8_t);
                if (!tileConfig.flags.uniform_tile_spacing_flag) {
                    for (int32_t j = 0; j < tileConfig.TileRows; j++) {
                        READ_PARAM(i, tileHeightInSbsMinus1[j], uint16_t);
                    }
                }
                READ_PARAM(i, tileConfig.context_update_tile_id, uint16_t);
            }
        } else if (args[i] == "--quant"){
            enableQuant = true;

            if (((i + 1) < argc) && (args[i + 1] == "--params")) {
                ++i;
                customQuantConfig = true;
                // parase quant params
                READ_PARAM(i, quantConfig.base_q_idx, uint8_t);
                READ_PARAM(i, quantConfig.DeltaQYDc, int8_t);
                READ_PARAM(i, quantConfig.DeltaQUDc, int8_t);
                READ_PARAM(i, quantConfig.DeltaQUAc, int8_t);
                READ_PARAM(i, quantConfig.flags.diff_uv_delta, bool); // must be 0 if separate_uv_delta_q = 0
                if (quantConfig.flags.diff_uv_delta) {
                    READ_PARAM(i, quantConfig.DeltaQVDc, int8_t);
                    READ_PARAM(i, quantConfig.DeltaQVAc, int8_t);
                }
                READ_PARAM(i, quantConfig.flags.using_qmatrix, bool);
                if (quantConfig.flags.using_qmatrix) {
                    READ_PARAM(i, quantConfig.qm_y, uint8_t);
                    READ_PARAM(i, quantConfig.qm_u, uint8_t);
                    READ_PARAM(i, quantConfig.qm_v, uint8_t);
                }
            }
        } else if (args[i] == "--lf"){
            enableLf = true;

            if (((i + 1) < argc) && (args[i + 1] == "--params")) {
                ++i;
                customLfConfig = true;
                // parase LF params
                READ_PARAM(i, lfConfig.loop_filter_level[0], uint8_t);
                READ_PARAM(i, lfConfig.loop_filter_level[1], uint8_t);
                if (lfConfig.loop_filter_level[0] || lfConfig.loop_filter_level[1]) {
                    READ_PARAM(i, lfConfig.loop_filter_level[2], uint8_t);
                    READ_PARAM(i, lfConfig.loop_filter_level[3], uint8_t);
                }
                READ_PARAM(i, lfConfig.loop_filter_sharpness, uint8_t);
                READ_PARAM(i, lfConfig.flags.loop_filter_delta_enabled, bool);
                if (lfConfig.flags.loop_filter_delta_enabled) {
                    READ_PARAM(i, lfConfig.flags.loop_filter_delta_update, bool);
                    if (lfConfig.flags.loop_filter_delta_update) {
                        READ_PARAM(i, lfConfig.update_ref_delta, uint8_t);
                        for (int32_t j = 0; j < STD_VIDEO_AV1_TOTAL_REFS_PER_FRAME; j++) {
                            READ_PARAM(i, lfConfig.loop_filter_ref_deltas[j], int8_t);
                        }
                        READ_PARAM(i, lfConfig.update_mode_delta, uint8_t);
                        for (int32_t j = 0; j < STD_VIDEO_AV1_LOOP_FILTER_ADJUSTMENTS; j++) {
                            READ_PARAM(i, lfConfig.loop_filter_mode_deltas[j], int8_t);
                        }
                    }
                }
            }
        } else if (args[i] == "--cdef"){
            enableCdef = true;

            if (((i + 1) < argc) && (args[i + 1] == "--params")) {
                ++i;
                customCdefConfig = true;
                // parse CDEF params
                READ_PARAM(i, cdefConfig.cdef_damping_minus_3, uint8_t);
                READ_PARAM(i, cdefConfig.cdef_bits, uint8_t);
                for (int32_t j = 0; j < (1 << cdefConfig.cdef_bits); j++) {
                    READ_PARAM(i, cdefConfig.cdef_y_pri_strength[j], uint8_t);
                    READ_PARAM(i, cdefConfig.cdef_y_sec_strength[j], uint8_t);
                    READ_PARAM(i, cdefConfig.cdef_uv_pri_strength[j], uint8_t);
                    READ_PARAM(i, cdefConfig.cdef_uv_sec_strength[j], uint8_t);
                }
            }
        } else if (args[i] == "--lr"){
            enableLr = true;

            // [ --params <type[0]> <...> <size[0]> <...> ]
            // Eg: --lr --params 2 2 2   1 1 1
            if (((i + 1) < argc) && (args[i + 1] == "--params")) {
                ++i;
                customLrConfig = true;
                // parse LR params
                for (int32_t j = 0; j < STD_VIDEO_AV1_MAX_NUM_PLANES; j++) {
                    READ_PARAM(i, lrConfig.FrameRestorationType[j], StdVideoAV1FrameRestorationType);
                }
                for (int32_t j = 0; j < STD_VIDEO_AV1_MAX_NUM_PLANES; j++) {
                    READ_PARAM(i, lrConfig.LoopRestorationSize[j], uint16_t);
                }
            }
        } else if (args[i] == "--profile"){
            if (++i >= argc) {
                fprintf(stderr, "invalid parameter for %s\n", args[i-1].c_str());
                return -1;
            }
            std::string prfl = args[i];
            if ( prfl == "0" || prfl == "main") {
                profile = STD_VIDEO_AV1_PROFILE_MAIN;
            } else if (prfl == "1" || prfl == "high") {
                profile = STD_VIDEO_AV1_PROFILE_HIGH;
            } else if (prfl == "2" || prfl == "professional") {
                profile = STD_VIDEO_AV1_PROFILE_PROFESSIONAL;
            } else {
                // Invalid profile
                fprintf(stderr, "Invalid profile: %s\n", prfl.c_str());
                return -1;
            }
        } else {
            fprintf(stderr, "Unrecognized option: %s\n", argv[i]);
            //printAV1Help();
            return -1;
        }
    }
    return 0;
}

bool EncoderConfigAV1::InitSequenceHeader(StdVideoAV1SequenceHeader *seqHdr)
{
    memset(seqHdr, 0, sizeof(StdVideoAV1SequenceHeader));

    seqHdr->max_frame_width_minus_1 = (uint16_t)(encodeWidth - 1);
    seqHdr->max_frame_height_minus_1 = (uint16_t)(encodeHeight - 1);
    seqHdr->frame_width_bits_minus_1 = (uint8_t)(std::max((int)FastIntLog2(encodeWidth) - 1, 0));
    seqHdr->frame_height_bits_minus_1 = (uint8_t)(std::max((int)FastIntLog2(encodeHeight) - 1, 0));
    seqHdr->flags.frame_id_numbers_present_flag = 0;
    seqHdr->additional_frame_id_length_minus_1 = FRAME_ID_BITS - DELTA_FRAME_ID_BITS - 1;
    seqHdr->delta_frame_id_length_minus_2 = DELTA_FRAME_ID_BITS - 2;
    seqHdr->flags.enable_order_hint = 1;
    seqHdr->order_hint_bits_minus_1 = ORDER_HINT_BITS - 1;
    seqHdr->flags.enable_cdef = enableCdef ? 1 : 0;
    seqHdr->flags.enable_restoration = enableLr ? 1 : 0;

    return true;
}

VkResult EncoderConfigAV1::InitDeviceCapabilities(const VulkanDeviceContext* vkDevCtx)
{
    VkResult result = VulkanVideoCapabilities::GetVideoEncodeCapabilities<VkVideoEncodeAV1CapabilitiesKHR, VK_STRUCTURE_TYPE_VIDEO_ENCODE_AV1_CAPABILITIES_KHR,
                                                                          VkVideoEncodeAV1QuantizationMapCapabilitiesKHR, VK_STRUCTURE_TYPE_VIDEO_ENCODE_AV1_QUANTIZATION_MAP_CAPABILITIES_KHR>
                                                        (vkDevCtx, videoCoreProfile,
                                                         videoCapabilities,
                                                         videoEncodeCapabilities,
                                                         av1EncodeCapabilities,
                                                         quantizationMapCapabilities,
                                                         av1QuantizationMapCapabilities);
    if (result != VK_SUCCESS) {
        std::cout << "*** Could not get video capabilities :" << result << " ***" << std::endl;
        assert(!"Coult not get Video Capabilities!");
        return result;
    }

    if (verboseMsg) {
        std::cout << "\t\t\t" << VkVideoCoreProfile::CodecToName(codec) << "encode capabilities: " << std::endl;
        std::cout << "\t\t\t" << "minBitstreamBufferOffsetAlignment: " << videoCapabilities.minBitstreamBufferOffsetAlignment << std::endl;
        std::cout << "\t\t\t" << "minBitstreamBufferSizeAlignment: " << videoCapabilities.minBitstreamBufferSizeAlignment << std::endl;
        std::cout << "\t\t\t" << "pictureAccessGranularity: " << videoCapabilities.pictureAccessGranularity.width << " x " << videoCapabilities.pictureAccessGranularity.height << std::endl;
        std::cout << "\t\t\t" << "minExtent: " << videoCapabilities.minCodedExtent.width << " x " << videoCapabilities.minCodedExtent.height << std::endl;
        std::cout << "\t\t\t" << "maxExtent: " << videoCapabilities.maxCodedExtent.width  << " x " << videoCapabilities.maxCodedExtent.height << std::endl;
        std::cout << "\t\t\t" << "maxDpbSlots: " << videoCapabilities.maxDpbSlots << std::endl;
        std::cout << "\t\t\t" << "maxActiveReferencePictures: " << videoCapabilities.maxActiveReferencePictures << std::endl;
    }

    return VK_SUCCESS;
}

int8_t EncoderConfigAV1::InitDpbCount()
{
    dpbCount = STD_VIDEO_AV1_NUM_REF_FRAMES + 1; // BUFFER_POOL_MAX_SIZE = Number of frames in buffer pool = 10
    return dpbCount;
}

bool EncoderConfigAV1::DetermineLevelTier()
{
    uint32_t frameRateNum = (frameRateNumerator > 0) ? frameRateNumerator : (uint32_t)FRAME_RATE_NUM_DEFAULT;
    uint32_t frameRateDenom = (frameRateDenominator > 0) ? frameRateDenominator : (uint32_t)FRAME_RATE_DEN_DEFAULT;
    uint32_t picSize = encodeWidth * encodeHeight;
    uint64_t displayRate = (frameRateNum * picSize) / frameRateDenom;
    uint64_t decodeRate = ((frameRateNum + 0) * picSize) / frameRateDenom;
    uint32_t headerRate = (frameRateNum + 0) / frameRateDenom;

    uint32_t lvl = STD_VIDEO_AV1_LEVEL_2_0;

    // TODO: how to choose tier
    tier = 0;

    for (; lvl <= STD_VIDEO_AV1_LEVEL_7_3; lvl++) {
        if (levelLimits[lvl].level == STD_VIDEO_AV1_LEVEL_INVALID) continue;

        level = (StdVideoAV1Level)lvl;

        if (picSize > levelLimits[lvl].maxPicSize) continue;
        if (encodeWidth > levelLimits[lvl].maxHSize) continue;
        if (encodeHeight > levelLimits[lvl].maxVSize) continue;
        if (displayRate > levelLimits[lvl].maxDisplayRate) continue;
        if (decodeRate > levelLimits[lvl].maxDecodeRate) continue;
        if (headerRate > levelLimits[lvl].maxHeaderRate) continue;

        // TODO: if ratecontrol params are specified at the command line use the below code
        // otherwise, use level max values.
        //if (hrdBitrate != 0) {
        //    if (lvl > STD_VIDEO_AV1_LEVEL_4_0) {
        //        tier = 1;
        //    }
        //    uint32_t lvlBitrate = GetLevelBitrate();
        //    if (hrdBitrate > lvlBitrate) continue;
        //}

        // double minCompressRatio = GetMinCompressRatio(decodeRate);
        // uint32_t compressedSize = 0; // TODO: How to estimate?
        // uint32_t uncompressedSize = GetUncompressedSize();
        // uint32_t compressedRatio = uncompressedSize / compressedSize;
        // if (compressedRatio < minCompressRatio) continue;

        break;
    }

    return true;
}

bool EncoderConfigAV1::InitRateControl()
{
    DetermineLevelTier();

    // use level max values for now. Limit it to 120Mbits/sec
    uint32_t levelBitrate = std::min(GetLevelBitrate(), 120000000u);

    // If no bitrate is specified, use the level limit
    if (averageBitrate == 0) {
        averageBitrate = hrdBitrate ? hrdBitrate : levelBitrate;
    }

    // If no HRD bitrate is specified, use 3x average for VBR (without going above level limit) or equal to average bitrate for
    // CBR
    if (hrdBitrate == 0) {
        if ((rateControlMode == VK_VIDEO_ENCODE_RATE_CONTROL_MODE_VBR_BIT_KHR) && (averageBitrate < levelBitrate)) {
            hrdBitrate = std::min(averageBitrate * 3, levelBitrate);
            // At least 500ms at peak rate if the application specifies the buffersize but not the HRD bitrate
            if (vbvBufferSize != 0) {
                hrdBitrate = std::min(hrdBitrate, std::max(vbvBufferSize * 2, averageBitrate));
            }
        } else {
            hrdBitrate = averageBitrate;
        }
    }

    // avg bitrate must not be higher than max bitrate,
    if (averageBitrate > hrdBitrate) {
        averageBitrate = hrdBitrate;
    }

    if (rateControlMode == VK_VIDEO_ENCODE_RATE_CONTROL_MODE_CBR_BIT_KHR) {
        hrdBitrate = averageBitrate;
    }

    // Use the level limit for the max VBV buffer size (1 second at MaxBitrate), and no more than 8 seconds at peak rate
    uint32_t maxVbvBufferSize = levelBitrate;
    vbvBufferSize = std::min(vbvBufferSize, maxVbvBufferSize);
    if (vbvBufferSize == 0) {
        vbvBufferSize = std::min(maxVbvBufferSize, 100000000u);
        if (rateControlMode != VK_VIDEO_ENCODE_RATE_CONTROL_MODE_DISABLED_BIT_KHR) {
            if ((vbvBufferSize >> 3) > hrdBitrate) {
                vbvBufferSize = hrdBitrate << 3;
            }
        }
    }

    if (vbvInitialDelay == 0) {
        // 90% occupancy or at least on second of fullness if possible
        vbvInitialDelay = std::max(vbvBufferSize - vbvBufferSize / 10, std::min(vbvBufferSize, hrdBitrate));
    } else if (vbvInitialDelay > vbvBufferSize) {
        vbvInitialDelay = vbvBufferSize;
    }

    minQIndex.intraQIndex        = av1EncodeCapabilities.minQIndex;
    minQIndex.predictiveQIndex   = av1EncodeCapabilities.minQIndex;
    minQIndex.bipredictiveQIndex = av1EncodeCapabilities.minQIndex;
    maxQIndex.intraQIndex        = av1EncodeCapabilities.maxQIndex;
    maxQIndex.predictiveQIndex   = av1EncodeCapabilities.maxQIndex;
    maxQIndex.bipredictiveQIndex = av1EncodeCapabilities.maxQIndex;

    return true;
}

bool EncoderConfigAV1::GetRateControlParameters(VkVideoEncodeRateControlInfoKHR* pRcInfo,
                                  VkVideoEncodeRateControlLayerInfoKHR* pRcLayerInfo,
                                  VkVideoEncodeAV1RateControlInfoKHR* pRcInfoAV1,
                                  VkVideoEncodeAV1RateControlLayerInfoKHR* pRcLayerInfoAV1)
{
    pRcLayerInfo->averageBitrate = averageBitrate;
    pRcLayerInfo->maxBitrate = hrdBitrate;
    pRcLayerInfo->frameRateNumerator = frameRateNumerator;
    pRcLayerInfo->frameRateDenominator = frameRateDenominator;

    if (rateControlMode == VK_VIDEO_ENCODE_RATE_CONTROL_MODE_DEFAULT_KHR) {
        pRcInfo->rateControlMode = VK_VIDEO_ENCODE_RATE_CONTROL_MODE_VBR_BIT_KHR;
    } else {
        pRcInfo->rateControlMode = rateControlMode;
    }
    pRcInfo->virtualBufferSizeInMs = (uint32_t)(vbvBufferSize * 1000ull / hrdBitrate);
    pRcInfo->initialVirtualBufferSizeInMs = (uint32_t)(vbvInitialDelay * 1000ull / hrdBitrate);

    if (pRcInfo->rateControlMode != VK_VIDEO_ENCODE_RATE_CONTROL_MODE_DISABLED_BIT_KHR) {
        pRcLayerInfoAV1->useMinQIndex = VK_TRUE;
        pRcLayerInfoAV1->minQIndex = minQIndex;
        pRcLayerInfoAV1->useMaxQIndex = VK_TRUE;
        pRcLayerInfoAV1->maxQIndex = maxQIndex;
    }

    pRcInfoAV1->gopFrameCount = gopStructure.GetGopFrameCount();
    pRcInfoAV1->keyFramePeriod = gopStructure.GetIdrPeriod();
    pRcInfoAV1->consecutiveBipredictiveFrameCount = gopStructure.GetConsecutiveBFrameCount();
    pRcInfoAV1->temporalLayerCount = 1;

    return true;
}