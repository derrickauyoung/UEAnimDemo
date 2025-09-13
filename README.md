# UEAnimDemo

This project demonstrates animation features in Unreal Engine.

## Prerequisites

- Unreal Engine (version 5.x recommended)
- Git (optional, for cloning the repository)

## Getting Started

1. **Clone the repository** (if applicable):
    ```sh
    git clone git@github.com:derrickauyoung/UEAnimDemo.git UEAnimDemo
    ```

2. **Open the project:**
    - Launch Unreal Engine.
    - Click on `Open Project`.
    - Navigate to the project folder and select `UEAnimDemo.uproject`.

3. **Build the project:**
    - On Mac OS:
    - /Users/Shared/Epic\ Games/UE_5.6/Engine/Build/BatchFiles/Mac/Build.sh UE_AnimDemoEditor Mac Development -project="${ROOT_DIR}/UE_AnimDemo.uproject" -waitmutex -NoHotReload

4. **Run the project:**
    - Click the `Play` button in the Unreal Editor to test the demo.
    - Use `W` to move the player forward
    - Use `A` to move the player left
    - Use `S` to move the player back
    - Use `D` to move the player right
    - Press `o` key to adjust Mouse Sensitivity/Smoothness/Invert-Y settings

## Troubleshooting

- Ensure all required plugins are enabled.
- If you encounter build errors, regenerate project files (`File > Generate Visual Studio project files`).

## License

This project is licensed under the MIT License.
