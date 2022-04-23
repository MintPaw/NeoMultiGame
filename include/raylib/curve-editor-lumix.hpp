#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"

#include <math.h>

// TAKEN FROM (with much cleaning + tweaking):
// https://github.com/nem0/LumixEngine/blob/39e46c18a58111cc3c8c10a4d5ebbb614f19b1b8/external/imgui/imgui_user.inl#L505-L930

static const float NODE_SLOT_RADIUS = 4.0f;

namespace ImGui
{

static ImVec2 node_pos;
static ImGuiID last_node_id;

enum class CurveEditorFlags
{
    NO_TANGENTS = 1 << 0,
    SHOW_GRID = 1 << 1,
    RESET = 1 << 2
};

int CurveEditor(const char* label, float* values, int points_count, const ImVec2& editor_size, ImU32 flags,
                int* new_count)
{
    enum class StorageValues : ImGuiID
    {
        FROM_X = 100,
        FROM_Y,
        WIDTH,
        HEIGHT,
        IS_PANNING,
        POINT_START_X,
        POINT_START_Y
    };

    const float HEIGHT = 100;
    static ImVec2 start_pan;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    ImVec2 size = editor_size;
    size.x = size.x < 0 ? CalcItemWidth() + (style.FramePadding.x * 2) : size.x;
    size.y = size.y < 0 ? HEIGHT : size.y;

    ImGuiWindow* parent_window = GetCurrentWindow();
    ImGuiID id = parent_window->GetID(label);
    if (!BeginChildFrame(id, size, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
    {
        EndChild();
        return -1;
    }

    int hovered_idx = -1;
    if (new_count)
        *new_count = points_count;

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
    {
        EndChild();
        return -1;
    }

    ImVec2 points_min(FLT_MAX, FLT_MAX);
    ImVec2 points_max(-FLT_MAX, -FLT_MAX);
    for (int point_idx = 0; point_idx < points_count; ++point_idx)
    {
        ImVec2 point;
        if (flags & (int)CurveEditorFlags::NO_TANGENTS)
        {
            point = ((ImVec2*)values)[point_idx];
        }
        else
        {
            point = ((ImVec2*)values)[1 + point_idx * 3];
        }
        points_max = ImMax(points_max, point);
        points_min = ImMin(points_min, point);
    }
    points_max.y = ImMax(points_max.y, points_min.y + 0.0001f);

    float from_x = window->StateStorage.GetFloat((ImGuiID)StorageValues::FROM_X, points_min.x);
    float from_y = window->StateStorage.GetFloat((ImGuiID)StorageValues::FROM_Y, points_min.y);
    float width = window->StateStorage.GetFloat((ImGuiID)StorageValues::WIDTH, points_max.x - points_min.x);
    float height = window->StateStorage.GetFloat((ImGuiID)StorageValues::HEIGHT, points_max.y - points_min.y);
    window->StateStorage.SetFloat((ImGuiID)StorageValues::FROM_X, from_x);
    window->StateStorage.SetFloat((ImGuiID)StorageValues::FROM_Y, from_y);
    window->StateStorage.SetFloat((ImGuiID)StorageValues::WIDTH, width);
    window->StateStorage.SetFloat((ImGuiID)StorageValues::HEIGHT, height);

    ImVec2 beg_pos = GetCursorScreenPos();

    const ImRect inner_bb = window->InnerRect;
    const ImRect frame_bb(inner_bb.Min - style.FramePadding, inner_bb.Max + style.FramePadding);

    auto transform = [&](const ImVec2& pos) -> ImVec2 {
        float x = (pos.x - from_x) / width;
        float y = (pos.y - from_y) / height;

        return ImVec2(inner_bb.Min.x * (1 - x) + inner_bb.Max.x * x, inner_bb.Min.y * y + inner_bb.Max.y * (1 - y));
    };

    auto invTransform = [&](const ImVec2& pos) -> ImVec2 {
        float x = (pos.x - inner_bb.Min.x) / (inner_bb.Max.x - inner_bb.Min.x);
        float y = (inner_bb.Max.y - pos.y) / (inner_bb.Max.y - inner_bb.Min.y);

        return ImVec2(from_x + width * x, from_y + height * y);
    };

    if (flags & (int)CurveEditorFlags::SHOW_GRID)
    {
        int exp;
        frexp(width / 5, &exp);
        float step_x = (float)ldexp(1.0, exp);
        int cell_cols = int(width / step_x);

        float x = step_x * int(from_x / step_x);
        for (int i = -1; i < cell_cols + 2; ++i)
        {
            ImVec2 a = transform({x + i * step_x, from_y});
            ImVec2 b = transform({x + i * step_x, from_y + height});
            window->DrawList->AddLine(a, b, 0x55000000);
            char buf[64];
            if (exp > 0)
            {
                ImFormatString(buf, sizeof(buf), " %d", int(x + i * step_x));
            }
            else
            {
                ImFormatString(buf, sizeof(buf), " %f", x + i * step_x);
            }
            window->DrawList->AddText(b, 0x55000000, buf);
        }

        frexp(height / 5, &exp);
        float step_y = (float)ldexp(1.0, exp);
        int cell_rows = int(height / step_y);

        float y = step_y * int(from_y / step_y);
        for (int i = -1; i < cell_rows + 2; ++i)
        {
            ImVec2 a = transform({from_x, y + i * step_y});
            ImVec2 b = transform({from_x + width, y + i * step_y});
            window->DrawList->AddLine(a, b, 0x55000000);
            char buf[64];
            if (exp > 0)
            {
                ImFormatString(buf, sizeof(buf), " %d", int(y + i * step_y));
            }
            else
            {
                ImFormatString(buf, sizeof(buf), " %f", y + i * step_y);
            }
            window->DrawList->AddText(a, 0x55000000, buf);
        }
    }

    if (ImGui::GetIO().MouseWheel != 0 && ImGui::IsItemHovered())
    {
        float scale = powf(2, ImGui::GetIO().MouseWheel);
        width *= scale;
        height *= scale;
        window->StateStorage.SetFloat((ImGuiID)StorageValues::WIDTH, width);
        window->StateStorage.SetFloat((ImGuiID)StorageValues::HEIGHT, height);
    }
    if (ImGui::IsMouseReleased(1))
    {
        window->StateStorage.SetBool((ImGuiID)StorageValues::IS_PANNING, false);
    }
    if (window->StateStorage.GetBool((ImGuiID)StorageValues::IS_PANNING, false))
    {
        ImVec2 drag_offset = ImGui::GetMouseDragDelta(1);
        from_x = start_pan.x;
        from_y = start_pan.y;
        from_x -= drag_offset.x * width / (inner_bb.Max.x - inner_bb.Min.x);
        from_y += drag_offset.y * height / (inner_bb.Max.y - inner_bb.Min.y);
        window->StateStorage.SetFloat((ImGuiID)StorageValues::FROM_X, from_x);
        window->StateStorage.SetFloat((ImGuiID)StorageValues::FROM_Y, from_y);
    }
    else if (ImGui::IsMouseDragging(1) && ImGui::IsItemHovered())
    {
        window->StateStorage.SetBool((ImGuiID)StorageValues::IS_PANNING, true);
        start_pan.x = from_x;
        start_pan.y = from_y;
    }

    int changed_idx = -1;
    for (int point_idx = points_count - 2; point_idx >= 0; --point_idx)
    {
        ImVec2* points;
        if (flags & (int)CurveEditorFlags::NO_TANGENTS)
        {
            points = ((ImVec2*)values) + point_idx;
        }
        else
        {
            points = ((ImVec2*)values) + 1 + point_idx * 3;
        }

        ImVec2 p_prev = points[0];
        ImVec2 tangent_last;
        ImVec2 tangent;
        ImVec2 p;
        if (flags & (int)CurveEditorFlags::NO_TANGENTS)
        {
            p = points[1];
        }
        else
        {
            tangent_last = points[1];
            tangent = points[2];
            p = points[3];
        }

        auto handlePoint = [&](ImVec2& p, int idx) -> bool {
            static const float SIZE = 3;

            ImVec2 cursor_pos = GetCursorScreenPos();
            ImVec2 pos = transform(p);

            SetCursorScreenPos(pos - ImVec2(SIZE, SIZE));
            PushID(idx);
            InvisibleButton("", ImVec2(2 * NODE_SLOT_RADIUS, 2 * NODE_SLOT_RADIUS));

            ImU32 col = IsItemActive() || IsItemHovered() ? GetColorU32(ImGuiCol_PlotLinesHovered)
                                                          : GetColorU32(ImGuiCol_PlotLines);

            window->DrawList->AddLine(pos + ImVec2(-SIZE, 0), pos + ImVec2(0, SIZE), col);
            window->DrawList->AddLine(pos + ImVec2(SIZE, 0), pos + ImVec2(0, SIZE), col);
            window->DrawList->AddLine(pos + ImVec2(SIZE, 0), pos + ImVec2(0, -SIZE), col);
            window->DrawList->AddLine(pos + ImVec2(-SIZE, 0), pos + ImVec2(0, -SIZE), col);

            if (IsItemHovered())
                hovered_idx = point_idx + idx;

            bool changed = false;
            if (IsItemActive() && IsMouseClicked(0))
            {
                window->StateStorage.SetFloat((ImGuiID)StorageValues::POINT_START_X, pos.x);
                window->StateStorage.SetFloat((ImGuiID)StorageValues::POINT_START_Y, pos.y);
            }

            if (IsItemHovered() || IsItemActive() && IsMouseDragging(0))
            {
                char tmp[64];
                ImFormatString(tmp, sizeof(tmp), "%0.2f, %0.2f", p.x, p.y);
                window->DrawList->AddText({pos.x, pos.y - GetTextLineHeight()}, 0xff000000, tmp);
            }

            if (IsItemActive() && IsMouseDragging(0))
            {
                pos.x = window->StateStorage.GetFloat((ImGuiID)StorageValues::POINT_START_X, pos.x);
                pos.y = window->StateStorage.GetFloat((ImGuiID)StorageValues::POINT_START_Y, pos.y);
                pos += ImGui::GetMouseDragDelta();
                ImVec2 v = invTransform(pos);

                p = v;
                changed = true;
            }
            PopID();

            SetCursorScreenPos(cursor_pos);
            return changed;
        };

        auto handleTangent = [&](ImVec2& t, const ImVec2& p, int idx) -> bool {
            static const float SIZE = 2;
            static const float LENGTH = 18;

            auto normalized = [](const ImVec2& v) -> ImVec2 {
                float len = 1.0f / sqrtf(v.x * v.x + v.y * v.y);
                return ImVec2(v.x * len, v.y * len);
            };

            ImVec2 cursor_pos = GetCursorScreenPos();
            ImVec2 pos = transform(p);
            ImVec2 tang = pos + normalized(ImVec2(t.x, -t.y)) * LENGTH;

            SetCursorScreenPos(tang - ImVec2(SIZE, SIZE));
            PushID(-idx);
            InvisibleButton("", ImVec2(2 * NODE_SLOT_RADIUS, 2 * NODE_SLOT_RADIUS));

            window->DrawList->AddLine(pos, tang, GetColorU32(ImGuiCol_PlotLines));

            ImU32 col = IsItemHovered() ? GetColorU32(ImGuiCol_PlotLinesHovered) : GetColorU32(ImGuiCol_PlotLines);

            window->DrawList->AddLine(tang + ImVec2(-SIZE, SIZE), tang + ImVec2(SIZE, SIZE), col);
            window->DrawList->AddLine(tang + ImVec2(SIZE, SIZE), tang + ImVec2(SIZE, -SIZE), col);
            window->DrawList->AddLine(tang + ImVec2(SIZE, -SIZE), tang + ImVec2(-SIZE, -SIZE), col);
            window->DrawList->AddLine(tang + ImVec2(-SIZE, -SIZE), tang + ImVec2(-SIZE, SIZE), col);

            bool changed = false;
            if (IsItemActive() && IsMouseDragging(0))
            {
                tang = GetIO().MousePos - pos;
                tang = normalized(tang);
                tang.y *= -1;

                t = tang;
                changed = true;
            }
            PopID();

            SetCursorScreenPos(cursor_pos);
            return changed;
        };

        PushID(point_idx);
        if ((flags & (int)CurveEditorFlags::NO_TANGENTS) == 0)
        {
            window->DrawList->AddBezierCurve(transform(p_prev), transform(p_prev + tangent_last),
                                             transform(p + tangent), transform(p), GetColorU32(ImGuiCol_PlotLines),
                                             1.0f, 20);
            if (handleTangent(tangent_last, p_prev, 0))
            {
                points[1] = ImClamp(tangent_last, ImVec2(0, -1), ImVec2(1, 1));
                changed_idx = point_idx;
            }
            if (handleTangent(tangent, p, 1))
            {
                points[2] = ImClamp(tangent, ImVec2(-1, -1), ImVec2(0, 1));
                changed_idx = point_idx + 1;
            }
            if (handlePoint(p, 1))
            {
                if (p.x <= p_prev.x)
                    p.x = p_prev.x + 0.001f;
                if (point_idx < points_count - 2 && p.x >= points[6].x)
                {
                    p.x = points[6].x - 0.001f;
                }
                points[3] = p;
                changed_idx = point_idx + 1;
            }
        }
        else
        {
            window->DrawList->AddLine(transform(p_prev), transform(p), GetColorU32(ImGuiCol_PlotLines), 1.0f);
            if (handlePoint(p, 1))
            {
                if (p.x <= p_prev.x)
                    p.x = p_prev.x + 0.001f;
                if (point_idx < points_count - 2 && p.x >= points[2].x)
                {
                    p.x = points[2].x - 0.001f;
                }
                points[1] = p;
                changed_idx = point_idx + 1;
            }
        }
        if (point_idx == 0)
        {
            if (handlePoint(p_prev, 0))
            {
                if (p.x <= p_prev.x)
                    p_prev.x = p.x - 0.001f;
                points[0] = p_prev;
                changed_idx = point_idx;
            }
        }
        PopID();
    }

    SetCursorScreenPos(inner_bb.Min);

    InvisibleButton("bg", inner_bb.Max - inner_bb.Min);

    if (ImGui::IsItemActive() && ImGui::IsMouseDoubleClicked(0) && new_count)
    {
        ImVec2 mp = ImGui::GetMousePos();
        ImVec2 new_p = invTransform(mp);
        ImVec2* points = (ImVec2*)values;

        if ((flags & (int)CurveEditorFlags::NO_TANGENTS) == 0)
        {
            points[points_count * 3 + 0] = ImVec2(-0.2f, 0);
            points[points_count * 3 + 1] = new_p;
            points[points_count * 3 + 2] = ImVec2(0.2f, 0);
            ;
            ++*new_count;

            auto compare = [](const void* a, const void* b) -> int {
                float fa = (((const ImVec2*)a) + 1)->x;
                float fb = (((const ImVec2*)b) + 1)->x;
                return fa < fb ? -1 : (fa > fb) ? 1 : 0;
            };

            qsort(values, points_count + 1, sizeof(ImVec2) * 3, compare);
        }
        else
        {
            points[points_count] = new_p;
            ++*new_count;

            auto compare = [](const void* a, const void* b) -> int {
                float fa = ((const ImVec2*)a)->x;
                float fb = ((const ImVec2*)b)->x;
                return fa < fb ? -1 : (fa > fb) ? 1 : 0;
            };

            qsort(values, points_count + 1, sizeof(ImVec2), compare);
        }
    }

    if (hovered_idx >= 0 && ImGui::IsMouseDoubleClicked(0) && new_count && points_count > 2)
    {
        ImVec2* points = (ImVec2*)values;
        --*new_count;
        if ((flags & (int)CurveEditorFlags::NO_TANGENTS) == 0)
        {
            for (int j = hovered_idx * 3; j < points_count * 3 - 3; j += 3)
            {
                points[j + 0] = points[j + 3];
                points[j + 1] = points[j + 4];
                points[j + 2] = points[j + 5];
            }
        }
        else
        {
            for (int j = hovered_idx; j < points_count - 1; ++j)
            {
                points[j] = points[j + 1];
            }
        }
    }

    EndChildFrame();
    RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, inner_bb.Min.y), label);
    return changed_idx;
}

} // namespace ImGui
