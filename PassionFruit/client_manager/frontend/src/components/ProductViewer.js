import React from 'react';
import {
  Box,
  Paper,
  Typography,
  Table,
  TableBody,
  TableCell,
  TableContainer,
  TableRow,
  CircularProgress,
  Card,
  CardMedia,
  CardContent
} from '@mui/material';

export default function ProductViewer({ product, productPath }) {
  console.log('ProductViewer props:', { product, productPath });

  if (!product) {
    console.log('No product data available');
    return (
      <Box sx={{ display: 'flex', justifyContent: 'center', p: 3 }}>
        <CircularProgress />
      </Box>
    );
  }

  if (product.type === 'error') {
    console.log('Product error:', product.error);
    return (
      <Paper sx={{ p: 2, textAlign: 'center' }}>
        <Typography color="error">
          {product.error || 'Failed to parse product data'}
        </Typography>
      </Paper>
    );
  }

  console.log('Rendering product data:', product);

  // Handle image display
  if (product.type === 'image/png') {
    return (
      <Box sx={{ width: '100%' }}>
        <Typography variant="subtitle2" color="text.secondary" gutterBottom>
          Path: {productPath}
        </Typography>
        <Card>
          <CardMedia
            component="img"
            image={product.data}
            alt="Product Image"
            sx={{ 
              maxHeight: '500px',
              objectFit: 'contain',
              backgroundColor: 'background.paper'
            }}
          />
          <CardContent>
            <Table size="small">
              <TableBody>
                <TableRow>
                  <TableCell component="th" scope="row">Dimensions</TableCell>
                  <TableCell>{product.width} × {product.height}</TableCell>
                </TableRow>
                <TableRow>
                  <TableCell component="th" scope="row">Mode</TableCell>
                  <TableCell>{product.mode}</TableCell>
                </TableRow>
              </TableBody>
            </Table>
          </CardContent>
        </Card>
      </Box>
    );
  }

  // Handle uint32 display
  return (
    <Box sx={{ width: '100%' }}>
      <Typography variant="subtitle2" color="text.secondary" gutterBottom>
        Path: {productPath}
      </Typography>
      <TableContainer component={Paper}>
        <Table>
          <TableBody>
            <TableRow>
              <TableCell component="th" scope="row">Type</TableCell>
              <TableCell>{product.type}</TableCell>
            </TableRow>
            <TableRow>
              <TableCell component="th" scope="row">Decimal</TableCell>
              <TableCell>{product.value}</TableCell>
            </TableRow>
            <TableRow>
              <TableCell component="th" scope="row">Hexadecimal</TableCell>
              <TableCell>{product.hex}</TableCell>
            </TableRow>
            <TableRow>
              <TableCell component="th" scope="row">Binary</TableCell>
              <TableCell sx={{ fontFamily: 'monospace', wordBreak: 'break-all' }}>
                {product.binary}
              </TableCell>
            </TableRow>
          </TableBody>
        </Table>
      </TableContainer>
    </Box>
  );
}
