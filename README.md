# acrylic
Math formula visualizer

## Functions
`^`: Exponent
* ex. `a^b`

`_`: Subscript
* ex. `a_b`

Exponent and subscript can be combined like this:
* `{a_b}^c`

`\frac{num}{denom}`: Display fraction
* ex. `\frac{1}{2}`

`\sum{lower bound}{upper bound}{expr}`: Summation
* ex. `\sum{x=1}{5}{x^2}`

`\int`: Integral
* ex. `{\int_a}^b x^2`

`\oint`: Closed integral
* ex. see integral usage

`\lim{bottom}`: Limit
* ex. `\lim{x \to \inf} x`

`\vec{var}`: Vector notation
* ex. `\vec{a}`

Identifiers can be grouped together by either not leaving whitespace or using `{}`.
* ex. `2^a+b` or `2^{a + b}` will put `a + b` in the exponent, while `2^a + b` will only raise 2 to a.

Some constants (`\pi`, `\inf`) and greek letters (`\theta`, `\phi`, `\delta`, `\lambda`) have been implemented as well.
