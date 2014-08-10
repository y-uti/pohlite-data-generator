<?php

abstract class RandGen
{
    private static $initialized = false;

    protected function __construct()
    {
        if (!self::$initialized) {
            stats_rand_setall(mt_rand(), mt_rand());
            self::$initialized = true;
        }
    }

    abstract function __invoke();

    abstract function mean();
}

class RUnif extends RandGen
{
    private $min;
    private $max;

    function __construct($min, $max)
    {
        parent::__construct();
        $this->min = $min;
        $this->max = $max;
    }

    function __invoke()
    {
        return stats_rand_gen_iuniform($this->min, $this->max);
    }

    function mean()
    {
        return ($this->min + $this->max) / 2;
    }
}

class RNorm extends RandGen
{
    private $mean;
    private $stddev;

    function __construct($mean, $stddev)
    {
        parent::__construct();
        $this->mean = $mean;
        $this->stddev = $stddev;
    }

    function __invoke()
    {
        return stats_rand_gen_normal($this->mean, $this->stddev);
    }

    function mean()
    {
        return $this->mean;
    }
}

class RGamma extends RandGen
{
    private $shape;
    private $scale;

    function __construct($shape, $scale)
    {
        parent::__construct();
        $this->shape = $shape;
        $this->scale = $scale;
    }

    function __invoke()
    {
        return stats_rand_gen_gamma(1 / $this->scale, $this->shape);
    }

    function mean()
    {
        return $this->shape * $this->scale;
    }
}

class RPois extends RandGen
{
    private $mean;

    function __construct($mean)
    {
        parent::__construct();
        $this->mean = $mean;
    }

    function __invoke()
    {
        return stats_rand_gen_ipoisson($this->mean);
    }

    function mean()
    {
        return $this->mean;
    }
}

function main()
{
    $options = parse_options();

    $m = $options['m'];
    $n = $options['n'];
    $qGen = $options['q'];
    $rGen = $options['r'];

    echo "$m\n";
    echo "$n\n";
    for ($i = 0; $i < $n; ++$i) {
        $qi = (int) max(1, $qGen());
        $ri = (int) max(1, $qi * $rGen());
        echo "$qi $ri\n";
    }
}

function parse_options()
{
    $opts = getopt("m:n:q:r:");

    $opts['n'] =
        (int) (array_key_exists('n', $opts) ? $opts['n'] : 50);
    $opts['q'] = parse_options_dist(
        array_key_exists('q', $opts) ? $opts['q'] : 'u:1:10000');
    $opts['r'] = parse_options_dist(
        array_key_exists('r', $opts) ? $opts['r'] : 'u:1:500');
    $opts['m'] =
        (int) (array_key_exists('m', $opts) ?
               $opts['m'] : $opts['n'] * $opts['q']->mean() / 2);

    return $opts;
}

function parse_options_dist($opt)
{
    $values = explode(':', $opt);
    switch ($values[0][0]) {
    case 'u':
        $min = (int) $values[1];
        $max = (int) $values[2];
        return new RUnif($min, $max);
    case 'n':
        $mean = (float) $values[1];
        $stddev = (float) $values[2];
        return new RNorm($mean, $stddev);
    case 'g':
        $shape = (float) $values[1];
        $scale = (float) $values[2];
        return new RGamma($shape, $scale);
    case 'p':
        $mean = (float) $values[1];
        return new RPois($mean);
    }
}

main();
