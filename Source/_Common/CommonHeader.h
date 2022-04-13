#pragma once

// A function that does not return void must return a value
#pragma warning(error: 4716)

// Discarding a [[nodiscard]] is an error
#pragma warning(error: 4834)
